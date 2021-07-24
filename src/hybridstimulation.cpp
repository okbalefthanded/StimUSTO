//
#include <QtDebug>
#include <QApplication>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
//
#include "hybridstimulation.h"
#include "facespeller.h"
#include "flashingspeller.h"
#include "hybrid.h"
#include "ovtk_stimulations.h"
//
HybridStimulation::HybridStimulation(Hybrid *hybridStimulation)
{
    m_hybridStimulaiton = new Hybrid();
    initERPspeller(hybridStimulation->m_ERPparadigm);
    initSSVEP(hybridStimulation->m_SSVEPparadigm);
    initAnimations();
}

HybridStimulation::HybridStimulation(Hybrid *hybridStimulation, Speller *ERPspeller, SsvepGL *ssvepGL)
{
    m_hybridStimulaiton = hybridStimulation;
    m_ERPspeller = ERPspeller;
    m_ssvepStimulation = ssvepGL;
    //
    initExternalComm();
    //
    m_ssvepStimulation->m_firstRun = false;
    m_ssvepStimulation->m_flickeringSequence = new RandomFlashSequence(1, 1);

    if(m_hybridStimulaiton->m_SSVEPparadigm->desiredPhrase() == "")
    {
        RandomFlashSequence *rfseq = new RandomFlashSequence(m_hybridStimulaiton->m_SSVEPparadigm->nrElements(),
                                                             m_hybridStimulaiton->m_SSVEPparadigm->nrSequences() / m_hybridStimulaiton->m_SSVEPparadigm->nrElements());
        m_hybridStimulaiton->m_SSVEPparadigm->setDesiredPhrase(rfseq->toString());
    }

    if(m_hybridStimulaiton->experimentMode() == operation_mode::CALIBRATION)
    {
        m_trials = 1;
    }
    else if(m_hybridStimulaiton->experimentMode() == operation_mode::COPY_MODE ||
            operation_mode::FREE_MODE)
    {
        m_trials = m_hybridStimulaiton->m_ERPparadigm->desiredPhrase().count();
        // qDebug()<< Q_FUNC_INFO << m_hybridStimulaiton->m_ERPparadigm->desiredPhrase();
    }
    /*
    else if(m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE)
    {

    }
    */

    connect(m_ERPspeller, SIGNAL(slotTerminated()), this, SLOT(switchState()) );
    connect(m_ssvepStimulation, SIGNAL(slotTerminated()), this, SLOT(switchState()));

    if(m_hybridStimulaiton->m_order == order::ERP_FIRST)
    {
        m_ssvepStimulation->hide();
        m_ERPspeller->show();
    }
    else
    {
        m_ERPspeller->hide();
        m_ssvepStimulation->setScreen(QGuiApplication::screens().last());
        m_ssvepStimulation->showFullScreen();
    }

    initAnimations();
}

void HybridStimulation::hybridPreTrial()
{
    // qDebug() << "[HYBRID PRETRIAL START]" << Q_FUNC_INFO;

    if(m_hybridStimulaiton->experimentMode() == operation_mode::CALIBRATION)
    {
        m_ERPspeller->setDesiredPhrase(m_hybridStimulaiton->m_ERPparadigm->desiredPhrase());
        m_ssvepStimulation->m_flickeringSequence->sequence = RandomFlashSequence::toSequence(m_hybridStimulaiton->m_SSVEPparadigm->desiredPhrase());
    }

    else if(m_hybridStimulaiton->experimentMode() == operation_mode::COPY_MODE)
    {
        m_ERPspeller->setDesiredPhrase(m_hybridStimulaiton->m_ERPparadigm->desiredPhrase()[m_currentTrial].toLower());
        if (m_hybridStimulaiton->m_SSVEPparadigm->experimentMode() == operation_mode::COPY_MODE)
        {
            m_ssvepStimulation->m_flickeringSequence->sequence[0] = m_hybridStimulaiton->m_SSVEPparadigm->desiredPhrase()[m_currentTrial].digitValue();
        }
        else
        {
            m_ssvepStimulation->m_flickeringSequence->sequence[0] = 0;
        }
    }

    else if(m_hybridStimulaiton->experimentMode() == operation_mode::FREE_MODE)
    {
        m_ERPspeller->setDesiredPhrase("");
        m_ssvepStimulation->m_flickeringSequence->sequence[0] = 0;
    }

    m_hybridState = trial_state::STIMULUS;
}

void HybridStimulation::startTrial()
{
    // qDebug() << "[HYBRID TRIAL START]" << Q_FUNC_INFO;

    if(m_hybridState == trial_state::PRE_TRIAL)
    {
        hybridPreTrial();
    }

    if(m_hybridState == trial_state::STIMULUS)
    {

        if(m_hybridStimulaiton->m_order == order::ERP_FIRST)
        {
            if(m_switchStimulation)
            {
                qDebug() << Q_FUNC_INFO << "ERP trial";
                swichStimWindows();
                // m_ERPspeller->startTrial();
            }
            else
            {
                qDebug() << Q_FUNC_INFO << "SSVEP trial";
                swichStimWindows();
                utils::wait(100); // pause before switching
                // m_ssvepStimulation->startTrial();
            }
        }

        else
        {
            if(m_switchStimulation)
            {
                swichStimWindows();
                //utils::wait(100);
                //m_ssvepStimulation->startTrial();
            }

            else
            {
                swichStimWindows();
                // utils::wait(100); // pause before switching
                // m_ERPspeller->startTrial();
            }
        }
    }

    if(m_hybridState == trial_state::POST_TRIAL)
    {
        hybridPostTrial();
    }
}

void HybridStimulation::switchState()
{
    //    qDebug() << Q_FUNC_INFO;

    if(!m_switchStimulation)
    {
        m_hybridState = trial_state::POST_TRIAL;
    }
    m_switchStimulation = !m_switchStimulation;

    startTrial();
}

void HybridStimulation::swichStimWindows()
{

    if(m_hybridStimulaiton->m_order == order::ERP_FIRST)
    {

        if(m_switchStimulation)
        {
            // qDebug()<< QTime::currentTime();
            m_ssvepStimulation->hide();

            // m_ERPspeller->setProperty("opactity", 1);
            // m_ERPspeller->setProperty("opacity", 0.7);
            // qDebug()<< QTime::currentTime();
            m_ERPspeller->show();
            // m_ERPspeller->showFullScreen();
            // m_ssvepStimulation->setOpacity(0.01);
        }
        else
        {
            /*
        QGraphicsOpacityEffect* fade_effect = new QGraphicsOpacityEffect();
        m_ERPspeller->setGraphicsEffect(fade_effect);
        QPropertyAnimation *animation = new QPropertyAnimation(fade_effect, "opacity");
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->setDuration(50);
        animation->setStartValue(0.7);
        animation->setEndValue(0.01);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
        */
            // m_ssvepStimulation->setOpacity(0.7);
            // qDebug()<< QTime::currentTime();
            // utils::wait(50);
            m_ssvepStimulation->setScreen(QGuiApplication::screens().last());
            m_ssvepStimulation->showFullScreen();
            // qDebug()<< QTime::currentTime();
            m_ERPspeller->hide();
            // qDebug()<< QTime::currentTime();
            // m_ERPspeller->setProperty("opacity", 0.01);
        }
    }
    else
    {
        if(m_switchStimulation)
        {
            if(m_currentTrial == 0)
            {
                m_ERPspeller->hide();
                m_ssvepStimulation->startTrial();
                return;
            }
            else
            {
                m_SSVEPanimation->start();
                m_ssvepStimulation->setScreen(QGuiApplication::screens().last());
                m_ssvepStimulation->showFullScreen();
                m_ERPspeller->hide();
            }
        }
        else
        {
            m_ERPanimation->start();
            // m_ERPspeller->show();
            m_ERPspeller->showFullScreen();
            m_ssvepStimulation->hide();
        }
    }
}

void HybridStimulation::initExternalComm()
{
    if(m_hybridStimulaiton->externalComm() == external_comm::ENABLED)
    {
        m_robotSocket = new QTcpSocket();
        //    m_robotSocket->connectToHost(QHostAddress("10.3.66.5"), m_robotPort);
        // m_robotSocket->connectToHost(QHostAddress("10.3.65.128"), m_robotPort);
        m_robotSocket->connectToHost(QHostAddress(m_hybridStimulaiton->externalAddress()), m_robotPort);
        if(m_robotSocket->waitForConnected())
        {
            qDebug() << "Robot Connection : State Connected";
        }
        else
        {
            qDebug() << "Robot Connection : State Not Connected";
        }
    }
}

void HybridStimulation::externalComm()
{
    if(m_hybridStimulaiton->externalComm() == external_comm::ENABLED)
    {
        qDebug() << "Sending Feedback to Robot";
        // m_hybridCommand = "12";
        if (!m_robotSocket->isOpen())
        {
            qDebug()<< "Not sending Feedback to Robot : Cannot send feedback socket is not open";
        }

        try
        {
            // m_hybridCommand = "12";
            std::string str = m_hybridCommand.toStdString();
            const char* p = str.c_str();

            QByteArray byteovStimulation;
            QDataStream streamovs(&byteovStimulation, QIODevice::WriteOnly);
            streamovs.setByteOrder(QDataStream::LittleEndian);
            //  streamovs << m_hybridCommand;
            streamovs.writeRawData(p, m_hybridCommand.length());
            m_robotSocket->write(byteovStimulation);
            m_robotSocket->waitForBytesWritten();

        }
        catch(...)
        {
            qDebug() <<"Send Feedback to Robot, Issue With writting Data";
        }

        qDebug() << "Recieve State from Robot...";
        m_robotSocket->waitForReadyRead(40000);

        QByteArray robotState = m_robotSocket->readAll();

        quint8 rState = robotState.toUInt();
        qDebug()<< Q_FUNC_INFO << "Robot State recieved " << rState;
        if(rState == robot_state::READY)
        {
            qDebug()<< "Correct State";
            m_hybridState = trial_state::PRE_TRIAL;
        }

    }
    //    qDebug() << Q_FUNC_INFO << "Hybrid Command: "<< m_hybridCommand;
    /*
    QByteArray Data;
    m_hybridCommand = "12";
    qDebug() << Q_FUNC_INFO << "Hybrid Command: "<< m_hybridCommand;
    Data = m_hybridCommand.toUtf8();
    qDebug() << Q_FUNC_INFO << "Data"<< Data;
    m_robotSocket->writeDatagram(Data, QHostAddress("10.3.66.5"), m_robotPort);
    */
    //
}

void HybridStimulation::hybridPostTrial()
{
    // qDebug() << "[HYBRID POST TRIAL]" << Q_FUNC_INFO;

    bool correct = false;
    bool doExternalComm = true;

    // recieve feedback from both paradigms:
    // m_ERPspeller->receiveFeedback();
    // m_ssvepStimulation->receiveFeedback();
    //
    if(m_hybridStimulaiton->experimentMode() == operation_mode::COPY_MODE ||
            m_hybridStimulaiton->experimentMode() == operation_mode::FREE_MODE)
    {
        m_ERPFeedback = m_ERPspeller->m_text;
        m_SSVEPFeedback += m_ssvepStimulation->m_sessionFeedback;
        // qDebug() << Q_FUNC_INFO << "ERP feedback: " << m_ERPFeedback;
        // qDebug() << Q_FUNC_INFO << "SSVEP feedback: " << m_SSVEPFeedback;
    }
    /*
    // Send and Recieve feedback to/from Robot if external communication is enabled
    if (m_hybridStimulaiton->m_SSVEPparadigm->controlMode() == control_mode::ASYNC)
    {

        if(m_SSVEPFeedback[m_currentTrial].digitValue() == 1)
        {
            m_SSVEPFeedback[m_currentTrial] = '0';

        }
        else
        {
            m_SSVEPFeedback[m_currentTrial] = QString::number(m_SSVEPFeedback[m_currentTrial].digitValue() - 1).at(0);

        }

    }
    */
    // m_hybridCommand = m_ERPFeedback[m_currentTrial] + m_SSVEPFeedback.at(m_currentTrial);
    m_hybridCommand = m_ERPFeedback[m_ERPFeedback.length() - 1] + m_SSVEPFeedback.at(m_SSVEPFeedback.length() - 1);
    correct = m_ssvepStimulation->isCorrect();
    // show feedback on ERP speller for 500 ms
    m_ssvepStimulation->hide();
    m_ERPspeller->show();
    m_ERPspeller->showFeedback(m_hybridCommand, correct);

    // check if correct feedback, send it to robot, otherwise
    // repeat until feedback is correct in COPY MODE
    if (m_hybridStimulaiton->externalComm() == external_comm::ENABLED)
    {
        // Send and Recieve feedback to/from Robot if external communication is enabled
        if (m_hybridStimulaiton->m_SSVEPparadigm->controlMode() == control_mode::ASYNC)
        {
            if(m_SSVEPFeedback[m_currentTrial].digitValue() == 1)
            {
                m_SSVEPFeedback[m_currentTrial] = '0';
            }
            else
            {
                m_SSVEPFeedback[m_currentTrial] = QString::number(m_SSVEPFeedback[m_currentTrial].digitValue() - 1).at(0);
            }
        }


        if(m_hybridStimulaiton->experimentMode() == operation_mode::COPY_MODE)
        {
            if(m_ERPFeedback[m_ERPFeedback.length() - 1] != m_ERPspeller->getDesiredPhrase())
            {
                doExternalComm = false;
                --m_currentTrial;
                //  qDebug()<< Q_FUNC_INFO << "ERP feedback :"<< m_ERPFeedback << "Desired "<< m_ERPspeller->getDesiredPhrase();
                //  qDebug() << Q_FUNC_INFO << "Repeat TRIAL: with: " << m_currentTrial;
            }
            else
            {
                doExternalComm = true;
            }
        }
    }

    else
    {
        // increase waiting time after feedabck by 500 or 1000 ms
        // only when control of external device is disabled
        utils::wait(1000);
    }

    if (m_hybridCommand[0] != '#' && doExternalComm)
    {
        externalComm();
    }
    //
    hybridPostTrialEnd();

}

void HybridStimulation::hybridPostTrialEnd()
{
    ++m_currentTrial;
    // qDebug() << Q_FUNC_INFO << "Repeat TRIAL: with: " << m_currentTrial;
    m_hybridState = trial_state::PRE_TRIAL;

    if(m_currentTrial < m_trials || m_hybridStimulaiton->experimentMode() == operation_mode::FREE_MODE)
        // if(m_currentTrial < m_trials)
    {

        //  qDebug()<< Q_FUNC_INFO << m_ERPFeedback << " " << m_ERPFeedback[m_ERPFeedback.length() - 1];
        if(m_ERPFeedback[m_ERPFeedback.length() - 1] == '5' && m_hybridStimulaiton->experimentMode() == operation_mode::FREE_MODE) // stop command in ERP
        {
            qDebug()<< "Terminate FREE experiment";
            terminateExperiment();
        }
        else
        {

            startTrial();
        }
    }

    else
    {
        if(m_hybridStimulaiton->experimentMode() == operation_mode::COPY_MODE)
        {
            m_ERPCorrect = (m_ERPspeller->m_correct / m_trials) * 100;
            m_SSVEPCorrect = (m_ssvepStimulation->m_correct / m_trials) * 100;
            qDebug()<< "Accuracy in ERP: " << m_ERPCorrect;
            qDebug()<< "Accuracy in SSVEP: "<< m_SSVEPCorrect;
        }

        terminateExperiment();
    }
}

void HybridStimulation::terminateExperiment()
{
    qDebug() << "Hybrid Experiment End";
    m_ERPspeller->sendMarker(OVTK_StimulationId_ExperimentStop);
    utils::wait(2000);
    // m_ERPspeller->close();
    // m_ssvepStimulation->close();
    // emit experimentEnd();
}

void HybridStimulation::initAnimations()
{

  m_ERPanimation = new QPropertyAnimation(m_ERPspeller, "geometry");
    m_ERPanimation->setDuration(500);
    m_ERPanimation->setStartValue(QRect(1366, 0, 1366, 768));
    m_ERPanimation->setEndValue(QRect(0, 0, 1366, 768));
    connect(m_ERPanimation, SIGNAL(finished()), m_ERPspeller, SLOT(startTrial()));

    m_SSVEPanimation = new QPropertyAnimation(m_ssvepStimulation, "x");
    m_SSVEPanimation->setDuration(500);
    m_SSVEPanimation->setStartValue(1366);
    m_SSVEPanimation->setEndValue(0);
    connect(m_SSVEPanimation, SIGNAL(finished()), m_ssvepStimulation, SLOT(startTrial()));
}

void HybridStimulation::initERPspeller(ERP *erp)
{
    int spellerType = erp->stimulationType();

    switch (spellerType)
    {
    case speller_type::FLASHING_SPELLER:
    {
        m_ERPspeller = new FlashingSpeller();
        // m_ERPspeller->initSpeller(erp);
        break;
    }
        // case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
    case speller_type::FACES_SPELLER:
    case speller_type::INVERTED_FACE:
    case speller_type::COLORED_FACE:
    case speller_type::INVERTED_COLORED_FACE:
    {
        m_ERPspeller = new FaceSpeller();
        //m_ERPspeller->initSpeller(erp);
        break;
    }
    }

    if(m_hybridStimulaiton->m_order == order::SSVEP_FIRST)
    {
        m_ERPspeller->hide();
    }
}

void HybridStimulation::initSSVEP(SSVEP *ssvep)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapInterval(1); // vsync on
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer); //
    // format.setVersion(3,3);
    format.setVersion(3,0); // ANGLE supports ES 3.0, higher versions raise exceptions


    m_ssvepStimulation = new SsvepGL(ssvep, 12346);
    m_ssvepStimulation->setFormat(format);

    if(QGuiApplication::screens().size() == 2)
    {
        m_ssvepStimulation->resize(utils::getScreenSize());
    }
    else
    {
        m_ssvepStimulation->resize(QSize(1366, 768)); //temporaty size;
    }

    if (m_hybridStimulaiton->m_order == order::ERP_FIRST)
    {
        m_ssvepStimulation->hide();
    }
}

HybridStimulation::~HybridStimulation()
{}
