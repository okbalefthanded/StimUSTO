//
#include <QWindow>
#include <QMessageBox>
#include <QGenericMatrix>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QHBoxLayout>
#include <QThread>
#include <QTime>
#include <QPropertyAnimation>
#include <QDateTime>
#include <QPixmap>
//
#include "speller.h"
#include "ui_speller.h"
#include "configpanel.h"
#include "ovtk_stimulations.h"
#include "randomflashsequence.h"
#include "utils.h"
//
Speller::Speller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Speller)
{

    ui->setupUi(this);
    this->setProperty("windowTitle", "ERP Speller");
    this->show();


    if(qApp->screens().count() == 2)
    {
        this->windowHandle()->setScreen(qApp->screens().last());
        this->showFullScreen();
    }

    this->setStyleSheet("background-color : black");

    createLayout();

    m_stimTimer = new QTimer(this);
    m_isiTimer = new QTimer(this);
    m_preTrialTimer = new QTimer(this);

    m_stimTimer->setTimerType(Qt::PreciseTimer);
    m_stimTimer->setSingleShot(true);
    m_stimTimer->setInterval(100); //default value

    m_isiTimer->setTimerType(Qt::PreciseTimer);
    m_isiTimer->setSingleShot(true);
    m_isiTimer->setInterval(100); //default value

    m_preTrialTimer->setTimerType(Qt::PreciseTimer);
    m_preTrialTimer->setInterval(1000);
    m_preTrialTimer->setSingleShot(true);

    connect( m_stimTimer, SIGNAL(timeout()), this, SLOT(pauseFlashing()) );
    connect( m_isiTimer, SIGNAL(timeout()), this, SLOT(startFlashing()) );
    connect( m_preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );

    /*
    if(m_ERP->experimentMode() == operation_mode::FREE_MODE)
    {
        qDebug() << "m_pretrailwait set to 0";
        m_preTrialWait = 0;
    }
    */

    m_feedbackSocket = new QUdpSocket(this);
    m_feedbackSocket->bind(QHostAddress::LocalHost, m_feedbackPort);


    m_state = trial_state::PRE_TRIAL;
}

void Speller::startTrial()
{
    //     qDebug()<< "[TRIAL START]" << Q_FUNC_INFO;

    if (m_state == trial_state::PRE_TRIAL)
    {
        preTrial();
    }

    if (m_state == trial_state::STIMULUS)
    {
        startFlashing();
    }
    else if (m_state == trial_state::POST_STIMULUS)
    {
        pauseFlashing();
    }
}

void Speller::startFlashing(){}

void Speller::pauseFlashing()
{

    // sendMarker(OVTK_StimulationId_VisualStimulationStop);

    this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
    /*

    QString stimName = ":/images/"
                       +
                       QString::number(m_flashingSequence->sequence[m_currentStimulation])
                       +
                       ".png)";

    QPalette pl = this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()->palette();

    QPixmap picMap(stimName);

    pl.setBrush(QPalette::Window, QBrush(picMap));


    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setPalette(pl);

    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setAutoFillBackground(true);
    */
    switchStimulationTimers();
    ++m_currentStimulation;


    if (m_currentStimulation >= m_flashingSequence->sequence.count())
    {
        ++m_currentLetter;
        m_isiTimer->stop();
        m_stimTimer->stop();

        utils::wait(1000);
        sendMarker(OVTK_StimulationId_TrialStop);
        m_state = trial_state::FEEDBACK;

        if(m_ERP->experimentMode() == operation_mode::COPY_MODE || m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            feedback();
        }
        else if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            postTrial();
        }
    }
}

void Speller::preTrial()
{
    //    qDebug()<< Q_FUNC_INFO;

    if(m_trials == 0)
    {
        sendMarker(OVTK_StimulationId_ExperimentStart);
    }

    if (m_preTrialCount == 0)
    {
        sendMarker(OVTK_StimulationId_TrialStart);
        m_flashingSequence = new RandomFlashSequence(m_nrElements, m_ERP->nrSequences());

        if (m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            highlightTarget();
            m_text += m_desiredPhrase[m_currentLetter];
            m_textRow->setText(m_text);
        }
        else if(m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {
            highlightTarget();
        }
        else if(m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            utils::wait(500);
        }
    }

    m_preTrialTimer->start();
    ++m_preTrialCount;

    if (m_preTrialCount > m_preTrialWait || m_ERP->experimentMode() == operation_mode::FREE_MODE)
    {
        refreshTarget();
        m_preTrialTimer->stop();
        m_preTrialCount = 0;
        m_state = trial_state::STIMULUS;
    }

}

void Speller::feedback()
{
    qDebug() << Q_FUNC_INFO;

    receiveFeedback();

    m_textRow->setText(m_text);

    if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
    {

        qDebug() << "Desired letter: "<<  m_desiredPhrase[m_currentLetter - 1];
        qDebug() << "Desired letter: "<<  m_text[m_currentLetter - 1];

        if(m_text[m_currentLetter - 1] == m_desiredPhrase[m_currentLetter - 1])
        {
            this->layout()->itemAt(m_currentTarget)->
                    widget()->setStyleSheet("QLabel { color : green; font: 40pt }");
            ++m_correct;
        }
        else
        {
            this->layout()->itemAt(m_currentTarget)->
                    widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");

        }
    }

    postTrial();
}

void Speller::postTrial()
{
    //    qDebug()<< Q_FUNC_INFO;

    ++m_trials;
    m_currentStimulation = 0;
    m_state = trial_state::PRE_TRIAL;
    // wait
    utils::wait(1000);
    refreshTarget();


    if (m_currentLetter >= m_desiredPhrase.length() &&
            m_desiredPhrase.length() != 1 &&
            (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
             m_ERP->experimentMode() == operation_mode::CALIBRATION)
            )
    {
        m_correct = (m_correct / m_desiredPhrase.length()) * 100;
        qDebug()<< "Accuracy on ERP session: " << m_correct;
        qDebug()<< "Experiment End, closing speller";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        utils::wait(2000);
        emit(slotTerminated());
        this->close();
    }
    else if(m_desiredPhrase.length() <= 1)
    {
        // qDebug() << "[POST TRIAL 1]" << Q_FUNC_INFO;
        m_currentLetter = 0;
        emit(slotTerminated());
        return;
    }
    else
    {
        startTrial();
    }

}

void Speller::receiveFeedback()
{
    //qDebug() << Q_FUNC_INFO;
    //qDebug() << "FEEDBACK SOCKET PORT" << m_feedbackPort;
    // wait for OV python script to write in UDP feedback socket
    utils::wait(500);
    QHostAddress sender;
    quint16 senderPort;
    QByteArray *buffer = new QByteArray();

    buffer->resize(m_feedbackSocket->pendingDatagramSize());
    // qDebug() << "buffer size" << buffer->size();

    //m_feedbackSocket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
    while(m_feedbackSocket->hasPendingDatagrams())
    {
        m_feedbackSocket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
    }

    //  feedback_socket->waitForBytesWritten();
    m_text += buffer->data();
    qDebug()<< Q_FUNC_INFO <<"Feedback Data" << buffer->data();
}

bool Speller::isTarget()
{
    int index = m_flashingSequence->sequence[m_currentStimulation] - 1;

    if(m_desiredPhrase[m_currentLetter] == m_presentedLetters[index][0])
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Speller::highlightTarget()
{
    int idx = 0;

    for (int i=0; i<m_rows; i++)
    {
        for (int j=0; j<m_cols; j++)
        {
            //                        idx++;
            //            if (desired_phrase[m_currentLetter] == letters[i][j]){
            //                currentTarget = idx;
            //                break;
            if(m_desiredPhrase[m_currentLetter] == m_presentedLetters[idx][0])
            {
                m_currentTarget = idx + 1;
                break;
            }
            idx++;
        }
    }

    this->layout()->itemAt(m_currentTarget)->
            widget()->setStyleSheet("QLabel { color : red; font: 60pt }");

}

void Speller::refreshTarget()
{
    this->layout()->itemAt(m_currentTarget)->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
}

void Speller::sendStimulationInfo()
{
    sendMarker(OVTK_StimulationId_VisualStimulationStart);
    sendMarker(config::OVTK_StimulationLabel_Base + m_flashingSequence->sequence[m_currentStimulation]);

    // send target marker
    if (m_ERP->experimentMode() == operation_mode::CALIBRATION ||
            m_ERP->experimentMode() == operation_mode::COPY_MODE)
    {
        if (isTarget())
        {
            // qDebug()<< desired_phrase[currentLetter];
            sendMarker(OVTK_StimulationId_Target);
        }
        else
        {
            sendMarker(OVTK_StimulationId_NonTarget);
        }
    }
}

void Speller::switchStimulationTimers()
{
    if(m_state == trial_state::STIMULUS)
    {
        m_stimTimer->start();
        m_isiTimer->stop();
        m_state = trial_state::POST_STIMULUS;
    }
    else if(trial_state::POST_STIMULUS)
    {
        m_stimTimer->stop();
        m_isiTimer->start();
        m_state = trial_state::STIMULUS;
    }
}

ERP *Speller::erp() const
{
    return m_ERP;
}

void Speller::setERP(ERP *erp)
{
    m_ERP = erp;
    setTimers(m_ERP->stimulationDuration(), m_ERP->breakDuration());
    setDesiredPhrase(m_ERP->desiredPhrase());
}

void Speller::setTimers(int t_stimulation, int t_isi)
{
    m_stimTimer->setInterval(t_stimulation);
    m_isiTimer->setInterval(t_isi);
}

void Speller::setDesiredPhrase(const QString &t_desiredPhrase)
{
    m_desiredPhrase = t_desiredPhrase;
}

void Speller::createLayout()
{
    // speller settings
    m_rows = 3;
    m_cols = 3;
    m_nrElements = m_rows * m_cols;
    //    this->matrix_height = 640;
    //    this->matrix_width = 480;
    //    this->setGeometry(0, 0, matrix_width, matrix_height);
    //    this->Mlayout = new MatrixLayout(qMakePair(6,6), 6, 6);
    //    QHBoxLayout *genLayout = new QHBoxLayout();
    QGridLayout *layout = new QGridLayout();

    m_textRow = new QLabel(this);
    m_textRow->setText(m_desiredPhrase);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignCenter);
    //    textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_textRow, 0, 0, 1, 0);

    int k = 1;
    QString stimName;
    QPixmap pic;
    int label_h, label_w;
    // add speller ellements
    for(int i=1; i<m_rows+1; i++)
    {
        for(int j=0; j<m_cols; j++)
        {
            QLabel *element = new QLabel(this);
            label_h = element->height() + 40;
            label_w = element->width() + 40;

            // element->setText(letters[i-1][j]);

             element->setText(QString::number(k));
             element->setStyleSheet("font: 40pt; color:gray");

            /*
            stimName = ":/images/" + QString::number(k) + ".png"; // directions images
            // stimName ="image: url(:/images/" + QString::number(k) + ".png)";
            pic = QPixmap(stimName);
            element->setPixmap(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));
            // element->setStyleSheet(stimName);
            */

            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element, i, j);
            m_presentedLetters.append(QString::number(k));
            k++;
            // presented_letters.append(letters[i-1][j]);
        }
    }

    this->setLayout(layout);
}

void Speller::refreshLayout()
{
    // TODO
}

Speller::~Speller()
{
    delete ui;
}


