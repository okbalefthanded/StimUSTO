//
#include <QtDebug>
#include <QApplication>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <algorithm>
//
#include "hybriderp.h"
#include "facespeller.h"
#include "flashingspeller.h"
#include "doubleerp.h"
#include "ovtk_stimulations.h"

//
HybridERP::HybridERP(DoubleERP *hybridStimulation)
{
    m_hybridStimulation = new DoubleERP();
    initFirstSpeller(hybridStimulation->m_1stParadigm);
    initSecondSpeller(hybridStimulation->m_2ndParadigm);
    initAnimations();
}

HybridERP::HybridERP(DoubleERP *hybridStimulation, Speller *FirstSpeller, Speller *SecondSpeller)
{
    m_hybridStimulation = hybridStimulation;
    m_FirstSpeller      = FirstSpeller;
    m_SecondSpeller     = SecondSpeller;
    //
    initExternalComm();
    //

    if(m_hybridStimulation->experimentMode() == operation_mode::CALIBRATION)
    {
        m_trials = 1;
    }
    else if(m_hybridStimulation->experimentMode() == operation_mode::COPY_MODE ||
            operation_mode::FREE_MODE)
    {

        m_trials = m_hybridStimulation->m_1stParadigm->desiredPhrase().count();
        //   qDebug()<< Q_FUNC_INFO << m_hybridStimulaiton->m_ERPparadigm->desiredPhrase();
    }
    /*
    else if(m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE)
    {

    }
    */

    connect(m_FirstSpeller,  SIGNAL(slotTerminated()), this, SLOT(switchState()));
    connect(m_SecondSpeller, SIGNAL(slotTerminated()), this, SLOT(switchState()));

    if(utils::screenCount() == 2)
    {
        m_FirstSpeller->move(-1366, 0);//quick hack
        m_SecondSpeller->move(-1366, 0);//quick hack
    }

    m_FirstSpeller->show();
    m_SecondSpeller->hide();

    initAnimations();
}

void HybridERP::hybridPreTrial()
{
    // qDebug() << "[HYBRID PRETRIAL START]" << Q_FUNC_INFO;

    if(m_hybridStimulation->experimentMode() == operation_mode::CALIBRATION)
    {
        m_ERPspeller->setDesiredPhrase(m_hybridStimulation->m_ERPparadigm->desiredPhrase());
        m_ssvepStimulation->m_flickeringSequence->sequence = RandomFlashSequence::toSequence(m_hybridStimulation->m_SSVEPparadigm->desiredPhrase());
    }

    else if(m_hybridStimulation->experimentMode() == operation_mode::COPY_MODE)
    {
        m_ERPspeller->setDesiredPhrase(m_hybridStimulation->m_ERPparadigm->desiredPhrase()[m_currentTrial].toLower());
        //  qDebug()<< Q_FUNC_INFO << m_hybridStimulation->m_ERPparadigm->desiredPhrase();
        if (m_hybridStimulation->m_SSVEPparadigm->experimentMode() == operation_mode::COPY_MODE)
        {
            m_ssvepStimulation->m_flickeringSequence->sequence[0] = m_hybridStimulation->m_SSVEPparadigm->desiredPhrase()[m_currentTrial].digitValue();
        }
        else
        {
            m_ssvepStimulation->m_flickeringSequence->sequence[0] = 0;
        }
    }

    else if(m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE)
    {
        m_ERPspeller->setDesiredPhrase("");
        m_ssvepStimulation->m_flickeringSequence->sequence[0] = 0;
    }

    m_hybridState = trial_state::STIMULUS;
}

void HybridERP::startTrial()
{
    // qDebug() << "[HYBRID TRIAL START]" << Q_FUNC_INFO;

    if(m_hybridState == trial_state::PRE_TRIAL)
    {
        hybridPreTrial();
    }

    if(m_hybridState == trial_state::STIMULUS)
    {
        if(m_hybridStimulation->m_order == order::ERP_FIRST)
        {
            if(m_switchStimulation)
            {
                //qDebug() << Q_FUNC_INFO << "ERP trial";
                swichStimWindows();
                // m_ERPspeller->startTrial();
            }
            else
            {
                // qDebug() << Q_FUNC_INFO << "SSVEP trial";
                swichStimWindows();
                // utils::wait(100); // pause before switching
                // m_ssvepStimulation->startTrial();
            }
        }
        // SSVEP stim first
        else
        {
            swichStimWindows();
        }
    }

    if(m_hybridState == trial_state::POST_TRIAL)
    {
        hybridPostTrial();
    }
}

void HybridERP::switchState()
{
    //    qDebug() << Q_FUNC_INFO;

    if(!m_switchStimulation)
    {
        m_hybridState = trial_state::POST_TRIAL;
    }
    m_switchStimulation = !m_switchStimulation;

    startTrial();
}

void HybridERP::swichStimWindows()
{
    // qDebug()<< Q_FUNC_INFO << m_currentTrial;
    QColor fbkColor = Qt::black;

    // ERP FIRST IN ORDER
    if(m_hybridStimulation->m_order ==   order::ERP_FIRST)
    {
        // ERP TRIAL
        if(m_switchStimulation)
        {
            if(m_currentTrial == 0)
            {
                m_ssvepStimulation->hide();
                m_ERPspeller->show();
                m_ERPspeller->startTrial();
                return;
            }
            else
            {
                m_ERPanimation->start();
                // m_ERPspeller->show();
                m_ERPspeller->showFullScreen();
                m_ssvepStimulation->hide();
            }
        }
        // SSVEP TRIAL
        else
        {
            m_ssvepStimulation->setScreen(QGuiApplication::screens().last());
            m_SSVEPanimation->start();
            m_ssvepStimulation->showFullScreen();
            m_ERPspeller->hide();
            m_ERPFeedback    = m_ERPspeller->m_text;
            // qDebug()<< m_ERPFeedback << " " << m_ERPspeller->Correct()<< "correct or no";
            if(m_ERPspeller->Correct())
            {
                fbkColor = Qt::red;
            }
            m_ssvepStimulation->setExternalFeedback(m_ERPFeedback[m_ERPFeedback.length() - 1].digitValue(), fbkColor);
        }
    }
    // SSVEP FIRST IN ORDER
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
                m_ssvepStimulation->setScreen(QGuiApplication::screens().last());
                m_SSVEPanimation->start();
                //  m_ssvepStimulation->setScreen(QGuiApplication::screens().last());
                m_ssvepStimulation->showFullScreen();
                m_ERPspeller->hide();

                // displacing the break from hybridpostTrial to here
                // utils::wait(1000); // useless
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

void HybridERP::initExternalComm()
{
    if(m_hybridStimulation->externalComm() == external_comm::ENABLED)
    {
        // create an external comm object
        m_externComm = new ExternComm(m_hybridStimulation->externalAddress(), 12347, m_hybridStimulation->externalComm());
    }

}

void HybridERP::externalComm()
{
    if(m_hybridStimulation->externalComm() == external_comm::ENABLED)
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
        qDebug()<< "Robot State recieved " << rState;
        if(rState == machine_state::READY)
        {
            qDebug()<< "Correct State";
            m_hybridState = trial_state::PRE_TRIAL;
        }

    }

}

void HybridERP::hybridPostTrial()
{
    // qDebug()<< Q_FUNC_INFO;
    bool correct = false;
    bool doExternalComm = true;
    QColor erpColor = Qt::black;

    if(m_hybridStimulation->experimentMode() == operation_mode::COPY_MODE ||
            m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE)
    {
        m_ERPFeedback    = m_ERPspeller->m_text;
        m_SSVEPFeedback += m_ssvepStimulation->m_sessionFeedback;
        if (m_ERPspeller->Correct())
        {
            erpColor = Qt::red;
        }
        // qDebug() << Q_FUNC_INFO << "ERP feedback: "   << m_ERPFeedback;
        // qDebug() << Q_FUNC_INFO << "SSVEP feedback: " << m_SSVEPFeedback;
    }

    // m_hybridCommand = m_ERPFeedback[m_currentTrial] + m_SSVEPFeedback.at(m_currentTrial);
    m_hybridCommand = m_ERPFeedback[m_ERPFeedback.length() - 1] + m_SSVEPFeedback.at(m_SSVEPFeedback.length() - 1);

    // show feedback on SSVEP
    if (m_hybridStimulation->m_order == order::ERP_FIRST)
    {
        // qDebug()<< m_ERPFeedback[m_ERPFeedback.length() - 1] << "  " << m_ERPFeedback[m_ERPFeedback.length() - 1].digitValue();
        m_ssvepStimulation->setExternalFeedback(m_ERPFeedback[m_ERPFeedback.length() - 1].digitValue(), erpColor);
    }

    // show feedback on ERP speller for 500 ms
    else
    {
        correct = m_ssvepStimulation->isCorrect();
        m_ssvepStimulation->hide();
        m_ERPspeller->show();
        m_ERPspeller->showFeedback(m_hybridCommand, correct);
    }

    // check if correct feedback, send it to robot, otherwise
    // repeat until feedback is correct in COPY MODE
    if (m_hybridStimulation->externalComm() == external_comm::ENABLED)
    {
        QString directionFeedback, directionDesired;
        // Send and Recieve feedback to/from Robot if external communication is enabled
        if (m_hybridStimulation->m_SSVEPparadigm->controlMode() == control_mode::ASYNC)
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

        if(m_hybridStimulation->experimentMode() == operation_mode::COPY_MODE)
        {
            // direction controlled by SSVEP
            if(m_hybridStimulation->m_ERPparadigm->stimulationType() == speller_type::SMALL)
            {
                directionFeedback = m_SSVEPFeedback[m_SSVEPFeedback.length() - 1];
                directionDesired  = m_hybridStimulation->m_SSVEPparadigm->desiredPhrase()[m_currentTrial];
                std::reverse(m_hybridCommand.begin(), m_hybridCommand.end());
            }
            // direction controlled by ERP
            else
            {
                directionFeedback = m_ERPFeedback[m_ERPFeedback.length() - 1];
                directionDesired  = m_ERPspeller->getDesiredPhrase();
            }

            if(directionFeedback != directionDesired)
            {
                doExternalComm = false;
                --m_currentTrial;
            }
            else
            {
                doExternalComm = true;
            }
        }

        if (m_hybridCommand[0] != '#' && doExternalComm)
        {
            // externalComm();
            m_externComm->communicate(m_hybridCommand);
        }

    }

    /*
    else
    {
        // increase waiting time after feedabck by 500 or 1000 ms (experimenting with 2000ms)
        // only when control of external device is disabled
        utils::wait(1000); // 1000
    }
    */

    /*
    if (m_hybridCommand[0] != '#' && doExternalComm)
    {
       // externalComm();
         m_externComm->communicate(m_hybridCommand);
    }
    */
    //
    hybridPostTrialEnd();
}

void HybridERP::hybridPostTrialEnd()
{
    ++m_currentTrial;

    m_hybridState = trial_state::PRE_TRIAL;

    // More trials to conduct
    if( (m_currentTrial < m_trials) || (m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE))
    {
        if(m_ERPFeedback[m_ERPFeedback.length() - 1] == '5' && m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE) // stop command in ERP
        {
            qDebug()<< "Terminate FREE experiment";
            terminateExperiment();
        }
        else
        {
            startTrial();
        }
    }

    // Experiment Ends
    else
    {
        if(m_hybridStimulation->experimentMode() == operation_mode::COPY_MODE)
        {
            m_ERPCorrect   = (m_ERPspeller->m_correct / m_trials) * 100;
            m_SSVEPCorrect = (m_ssvepStimulation->m_correct / m_trials) * 100;
            qDebug()<< "Accuracy in ERP: " << m_ERPCorrect;
            qDebug()<< "Accuracy in SSVEP: "<< m_SSVEPCorrect;
        }

        terminateExperiment();
    }
}

void HybridERP::terminateExperiment()
{
    qDebug() << "Hybrid Experiment End";
    m_ERPspeller->sendMarker(OVTK_StimulationId_ExperimentStop);
    if (m_hybridStimulation->externalComm() == external_comm::ENABLED)
    {
        m_externComm->communicate("00");
    }
    utils::wait(2000);
    // m_ERPspeller->close();
    // m_ssvepStimulation->close();
    // emit experimentEnd();
}

void HybridERP::initAnimations()
{
    m_ERPanimation = new QPropertyAnimation(m_ERPspeller, "windowOpacity");
    m_ERPanimation->setDuration(750); //1000 //2000 //500 //250
    m_ERPanimation->setStartValue(0.0); //
    m_ERPanimation->setEndValue(1.0); //
    m_ERPanimation->setEasingCurve(QEasingCurve::OutQuint);
    connect(m_ERPanimation, SIGNAL(finished()), m_ERPspeller, SLOT(startTrial()));

    m_SSVEPanimation = new QPropertyAnimation(m_ssvepStimulation, "opacity");
    m_SSVEPanimation->setDuration(750); //1000  //500 //250
    m_SSVEPanimation->setStartValue(0.0); //1366
    m_SSVEPanimation->setEndValue(1.0); // 0
    m_SSVEPanimation->setEasingCurve(QEasingCurve::OutQuint);
    connect(m_SSVEPanimation, SIGNAL(finished()), m_ssvepStimulation, SLOT(startTrial()));
}

void HybridERP::initERPspeller(ERP *erp)
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
        // TODO: add the rest of Spellers
    }

    if(m_hybridStimulation->m_order == order::SSVEP_FIRST)
    {
        m_ERPspeller->hide();
    }
}

void HybridERP::initSSVEP(SSVEP *ssvep)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapInterval(1); // vsync on
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer); //
    // format.setVersion(3,3);
    format.setVersion(3,0); // ANGLE supports ES 3.0, higher versions raise exceptions


    // m_ssvepStimulation = new SsvepGL(ssvep, 12346);
    // m_ssvepStimulation = new SsvepCircle(ssvep, 12346);
    m_ssvepStimulation = new SsvepDirection(ssvep, 12346);
    m_ssvepStimulation->setFormat(format);

    if(QGuiApplication::screens().size() == 2)
    {
        m_ssvepStimulation->resize(utils::getScreenSize());
    }
    else
    {
        m_ssvepStimulation->resize(QSize(1366, 768)); //temporaty size;
    }

    if (m_hybridStimulation->m_order == order::ERP_FIRST)
    {
        m_ssvepStimulation->hide();
    }
}

HybridERP::~HybridStimulation(){}
