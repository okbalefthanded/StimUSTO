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

    m_feedbackSocket = new QUdpSocket(this);
    m_feedbackSocket->bind(QHostAddress::LocalHost, m_feedbackPort);

    m_state = trial_state::PRE_TRIAL;
}

void Speller::startTrial()
{
    qDebug()<< "[TRIAL START]" << Q_FUNC_INFO;

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
    //      qDebug()<< Q_FUNC_INFO;
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

    switchStimulationTimers();
    //    qDebug("Isi Timer started");
    ++m_currentStimulation;


    if (m_currentStimulation >= m_flashingSequence->sequence.count())
    {
        ++m_currentLetter;
        m_isiTimer->stop();
        m_stimTimer->stop();

        wait(1000);
        sendMarker(OVTK_StimulationId_TrialStop);
        m_state = trial_state::FEEDBACK;

        if(m_spellingMode == operation_mode::COPY_MODE || m_spellingMode == operation_mode::FREE_MODE){
            feedback();
        }
        else if(m_spellingMode == operation_mode::CALIBRATION)
        {
            postTrial();
        }
    }
}

void Speller::preTrial()
{
    qDebug()<< Q_FUNC_INFO;

    if (m_preTrialCount == 0)
    {
        m_flashingSequence = new RandomFlashSequence(m_nrElements, m_nrSequence);
        sendMarker(OVTK_StimulationId_TrialStart);

        if (m_spellingMode == operation_mode::CALIBRATION)
        {
            qDebug()<< "highlightTarget";
            highlightTarget();
            m_text += m_desiredPhrase[m_currentLetter];
            m_textRow->setText(m_text);
        }
        else if(m_spellingMode == operation_mode::COPY_MODE)
        {
            highlightTarget();
        }
    }

    m_preTrialTimer->start();
    ++m_preTrialCount;

    if (m_preTrialCount > m_preTrialWait)
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


    if (m_spellingMode == operation_mode::COPY_MODE)
    {

        if(m_text[m_currentLetter - 1] == m_desiredPhrase[m_currentLetter - 1])
        {
            this->layout()->itemAt(m_currentTarget)->
                    widget()->setStyleSheet("QLabel { color : green; font: 40pt }");
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
    qDebug()<< Q_FUNC_INFO;

    m_currentStimulation = 0;
    m_state = trial_state::PRE_TRIAL;
    // wait
    wait(1000);

    refreshTarget();

    if (m_currentLetter >= m_desiredPhrase.length() &&
            (m_spellingMode == operation_mode::COPY_MODE ||
             m_spellingMode == operation_mode::CALIBRATION))
    {
        qDebug()<< "Experiment End";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        wait(2000);
        this->close();
    }
    else
    {
        startTrial();
    }

}

void Speller::receiveFeedback()
{
    // wait for OV python script to write in UDP feedback socket
    wait(200);
    QHostAddress sender;
    quint16 senderPort;
    QByteArray *buffer = new QByteArray();

    buffer->resize(m_feedbackSocket->pendingDatagramSize());
    // qDebug() << "buffer size" << buffer->size();

    m_feedbackSocket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
    //    feedback_socket->waitForBytesWritten();
    m_text += buffer->data();
    // qDebug()<< "Feedback Data" << buffer->data();
}

bool Speller::isTarget()
{
    //    int row, column;
    int index = m_flashingSequence->sequence[m_currentStimulation] - 1;
    //    row = index / nr_elements;
    //    column = index % nr_elements;


    //    if(desired_phrase[m_currentLetter]==presented_letters[index])
    //    {
    //        //        qDebug()<< "letter : " << letters[row][column];
    //        //        qDebug()<< "desired letter: " << desired_phrase[m_currentLetter];
    //        //        qDebug()<< "flashing: "<< flashingSequence->sequence[m_currentStimulation];
    //        //        qDebug()<< "presented letter:" << presented_letters[index];
    //        //        qDebug()<< "row: " << row << " column: "<< column;
    //        return true;
    //    }
    //    else
    //    {
    //        return false;
    //    }

    if(m_desiredPhrase[m_currentLetter] == m_presentedLetters[index][0])
    {
        //                qDebug()<< "letter : " << letters[row][column];
//        qDebug()<< "desired letter: " << m_desiredPhrase[m_currentLetter];
        //        qDebug()<< "flashing: "<< flashingSequence->sequence[m_currentStimulation];
//        qDebug()<< "presented letter:" << m_presentedLetters[index];
        //        qDebug()<< "row: " << row << " column: "<< column;
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

void Speller::wait(int t_millisecondsToWait)
{

    qDebug()<< Q_FUNC_INFO;

    // from stackoverflow question:
    // http://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
    QTime dieTime = QTime::currentTime().addMSecs( t_millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        //        qDebug()<<"waiting..."<<QTime::currentTime();
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100);

    }
}

void Speller::sendStimulationInfo()
{
    sendMarker(OVTK_StimulationId_VisualStimulationStart);
    sendMarker(config::OVTK_StimulationLabel_Base + m_flashingSequence->sequence[m_currentStimulation]);

    // send target marker
    if (m_spellingMode == operation_mode::CALIBRATION ||
            m_spellingMode == operation_mode::COPY_MODE)
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

/* Setters */
void Speller::setSpellerType(int t_spellerType)
{
    m_spellerType = t_spellerType;
}

void Speller::setFeedbackPort(quint16 t_feedbackPort)
{
    m_feedbackPort = t_feedbackPort;
}

void Speller::setStimulationDuration(int t_stimDuration)
{
    m_stimulationDuration = t_stimDuration;
    m_stimTimer->setInterval(m_stimulationDuration);
}

void Speller::setDesiredPhrase(const QString &t_desiredPhrase)
{
    m_desiredPhrase = t_desiredPhrase;
}

void Speller::setSpellingMode(int t_spellingMode)
{
    m_spellingMode = t_spellingMode;
}

void Speller::setNrTrials(int t_nrTrials)
{
    m_nrTrials = t_nrTrials;
}

void Speller::setNrSequence(int t_nrSequence)
{
    m_nrSequence = t_nrSequence;
}

void Speller::setIsi(int t_isi)
{
    m_isi = t_isi;
    m_isiTimer->setInterval(t_isi);
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
    //        this->Mlayout = new MatrixLayout(qMakePair(6,6), 6, 6);
    //    QHBoxLayout *genLayout = new QHBoxLayout();
    QGridLayout *layout = new QGridLayout();

    m_textRow = new QLabel(this);
    m_textRow->setText(m_desiredPhrase);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignCenter);
    //    textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_textRow, 0, 0, 1, 0);

    int k = 1;
    // add speller ellements
    for(int i=1; i<m_rows+1; i++)
    {
        for(int j=0; j<m_cols; j++)
        {
            QLabel *element = new QLabel(this);
            //            element->setText(letters[i-1][j]);
            element->setText(QString::number(k));
            element->setStyleSheet("font: 40pt; color:gray");
            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element, i, j);
            m_presentedLetters.append(QString::number(k));
            k++;
            //            presented_letters.append(letters[i-1][j]);
        }
    }

    this->setLayout(layout);
}

void Speller::refreshLayout()
{

}

Speller::~Speller()
{
    delete ui;
}

void Speller::initSpeller(Paradigm *prdg){}
