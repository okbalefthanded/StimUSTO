//
#include <QLayout>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QWindow>
#include <QtWidgets>
//
#include "arabicspeller.h"
#include "erp.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//
ArabicSpeller::ArabicSpeller(QWidget *parent) : Speller(0)
{
    qDebug()<< Q_FUNC_INFO;

    setupUi(this);
    this->setProperty("windowTitle", "ERP Arabic Speller");
    this->show();

    if(qApp->screens().count() == 2)
    {
        this->windowHandle()->setScreen(qApp->screens().last());
        this->showFullScreen();
    }
    else
    {
        this->showMaximized();
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


void ArabicSpeller::pauseFlashing()
{
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

    switchStimulationTimers();
    ++m_currentStimulation;


    if (m_currentStimulation >= m_flashingSequence->sequence.count())
    {
        ++m_currentLetter;
        m_isiTimer->stop();
        m_stimTimer->stop();

        // utils::wait(1000); // time window after last epoch/stim
        // utils::wait(500);
        utils::wait(700); // 700 ms == epoch time windows
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

void ArabicSpeller::startFlashing()
{
    // qDebug()<< Q_FUNC_INFO;
    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");

    switchStimulationTimers();
}


void ArabicSpeller::createLayout()
{

    qDebug()<< Q_FUNC_INFO;
    // speller settings
    // Arabic speller
    m_rows = 5;
    m_cols = 8;
    m_nrElements = m_rows * m_cols;

    m_icons = QList<QPixmap>();
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);
    QGridLayout *layout = new QGridLayout();

    m_textRow = new QLabel(this);
    m_textRow->setText(m_desiredPhrase);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignLeft);
    //    textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_textRow, 0, 0, 1, 0);

    int k = 0, nbr=0;
    QString stimName;
    QPixmap pic;
    QImage iconImage;
    int label_h, label_w;
    // add speller ellements
    for(int i=1; i<m_rows+1; i++)
    {
        for(int j=0; j<m_cols; j++)
        {
            QLabel *element = new QLabel(this);
            label_h = element->height();
            label_w = element->width();

            if(k < utils::ArabicLetters.length())
            {
                element->setText(utils::ArabicLetters.at(k));
                m_presentedLetters.append(utils::ArabicLetters.at(k));
            }
            else
            {
                element->setText(utils::numbers.at(nbr));
                m_presentedLetters.append(utils::numbers.at(nbr));
                nbr++;
            }

            element->setStyleSheet("font: 40pt; color:gray");
            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element, i, j);

            k++;

        }
    }

    this->setLayout(layout);

}

void ArabicSpeller::highlightTarget()
{
    qDebug() << Q_FUNC_INFO;

    int idx = 0;

    for (int i=0; i<m_rows; i++)
    {
        for (int j=0; j<m_cols; j++)
        {
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

void ArabicSpeller::refreshTarget()
{
    this->layout()->itemAt(m_currentTarget)->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
}


ArabicSpeller::~ArabicSpeller()
{

}

void ArabicSpeller::preTrial()
{
    qDebug()<< Q_FUNC_INFO;

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
        if( m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {

            refreshTarget();
        }
        m_preTrialTimer->stop();
        m_preTrialCount = 0;
        m_state = trial_state::STIMULUS;
    }

}

void ArabicSpeller::feedback()
{
    qDebug() << Q_FUNC_INFO;

    receiveFeedback();

    m_textRow->setText(m_text);

    if (m_text[m_text.length()-1] != "#")
    {

        if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {

            int id = m_text[m_text.length()-1].digitValue();

            if( m_text[m_text.length()-1] == m_desiredPhrase[m_currentLetter - 1])
            {
                this->layout()->itemAt(m_currentTarget)->
                        widget()->setStyleSheet("QLabel { color : green; font: 40pt }");

                isCorrect = true;
                ++m_correct;
            }
            else
            {
                this->layout()->itemAt(m_currentTarget)->
                        widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");

                isCorrect = false;
            }
        }

        else if (m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            this->layout()->itemAt(0)->
                    widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");
        }
    }
    postTrial();
}

void ArabicSpeller::postTrial()
{
    qDebug()<< Q_FUNC_INFO;

    ++m_trials;

    utils::wait(250);

    if (m_text[m_text.length()-1] != "#")
    {
        if (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            refreshTarget();

        }
        else if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            refreshTarget();
        }
    }
    //
    // Send and Recieve feedback to/from Robot if external communication is enabled
    m_hybridCommand = m_text[m_text.length()-1] + "2";
    if(m_ERP->externalComm() == external_comm::ENABLED)
    {
        qDebug() << "Sending Feedback to Robot";
        if (!m_robotSocket->isOpen())
        {
            qDebug()<< "Not sending Feedback to Robot : Cannot send feedback socket is not open";
        }

        try
        {
            // m_hybridCommand = "12";
            std::string str = m_hybridCommand.toStdString();
            const char* p = str.c_str();
            qDebug()<< "command to send to Robot: " << m_hybridCommand;
            QByteArray byteovStimulation;
            QDataStream streamovs(&byteovStimulation, QIODevice::WriteOnly);
            streamovs.setByteOrder(QDataStream::LittleEndian);
            streamovs.writeRawData(p, m_hybridCommand.length());
            m_robotSocket->write(byteovStimulation);
            m_robotSocket->waitForBytesWritten();
        }
        catch(...)
        {
            qDebug() <<"Send Feedback to Robot, Issue With writting Data";
        }

        qDebug() << "Recieve State from Robot";
        m_robotSocket->waitForReadyRead();

        QByteArray robotState = m_robotSocket->readAll();

        quint8 rState = robotState.toUInt();
        qDebug()<< Q_FUNC_INFO << "Robot State recieved " << rState;
        if(rState == robot_state::READY)
        {
            qDebug()<< "Correct State";
            m_state = trial_state::PRE_TRIAL;
        }

    }

    m_currentStimulation = 0;
    m_state = trial_state::PRE_TRIAL;
    //
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
