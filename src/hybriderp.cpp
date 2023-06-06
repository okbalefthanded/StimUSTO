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
    }

    //TODO
    //else if(m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE)
    //{

    //}

    connect(m_FirstSpeller,  SIGNAL(slotTerminated()), this, SLOT(switchState()));
    connect(m_SecondSpeller, SIGNAL(slotTerminated()), this, SLOT(switchState()));

    if(utils::screenCount() == 2)
    {
        //  m_FirstSpeller->move(-1366, 0);//quick hack
        //  m_SecondSpeller->move(-1366, 0);//quick hack
        m_FirstSpeller->move(-1920, 0);//quick hack
        m_SecondSpeller->move(-1920, 0);//quick hack
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
        m_FirstSpeller->setDesiredPhrase(m_hybridStimulation->m_1stParadigm->desiredPhrase());
        m_SecondSpeller->setDesiredPhrase(m_hybridStimulation->m_2ndParadigm->desiredPhrase());
    }

    else if(m_hybridStimulation->experimentMode() == operation_mode::COPY_MODE)
    {
        m_FirstSpeller->setDesiredPhrase(m_hybridStimulation->m_1stParadigm->desiredPhrase()[m_currentTrial].toLower());
        m_SecondSpeller->setDesiredPhrase(m_hybridStimulation->m_2ndParadigm->desiredPhrase()[m_currentTrial].toLower());
        //  qDebug()<< Q_FUNC_INFO << m_hybridStimulation->m_ERPparadigm->desiredPhrase();
    }

    else if(m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE)
    {
        m_FirstSpeller->setDesiredPhrase("");
        m_SecondSpeller->setDesiredPhrase("");
    }

    m_hybridState = trial_state::STIMULUS;
}

void HybridERP::startTrial()
{
    //  qDebug() << "[HYBRID TRIAL START]" << Q_FUNC_INFO;

    if(m_hybridState == trial_state::PRE_TRIAL)
    {
        hybridPreTrial();
    }

    if(m_hybridState == trial_state::STIMULUS)
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

    // 1st ERP TRIAL
    if(m_switchStimulation)
    {
        if(m_currentTrial == 0)
        {
            m_SecondSpeller->hide();
            m_FirstSpeller->show();
            m_FirstSpeller->startTrial();
            return;
        }
        else
        {
            m_FirstAnimation->start();
            m_FirstSpeller->showFullScreen();
            m_SecondSpeller->hide();
        }
    }
    // 2nd ERP TRIAL
    else
    {
        m_SecondAnimation->start();

        m_SecondSpeller->showFullScreen();
        m_FirstSpeller->hide();
        m_FirstFeedback    = m_FirstSpeller->m_text;
        // qDebug()<< m_ERPFeedback << " " << m_ERPspeller->Correct()<< "correct or no";
        if(m_FirstSpeller->Correct())
        {
            fbkColor = Qt::red;
        }
        //   m_ssvepStimulation->setExternalFeedback(m_ERPFeedback[m_ERPFeedback.length() - 1].digitValue(), fbkColor);
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
        m_FirstFeedback  = m_FirstSpeller->m_text;
        m_SecondFeedback = m_SecondSpeller->m_text;

        if (m_FirstSpeller->Correct())
        {
            erpColor = Qt::red;
        }
    }

    // reversed feedback
    m_hybridCommand.append(m_SecondFeedback.at(m_currentTrial));
    m_hybridCommand.append(m_FirstFeedback.at(m_currentTrial));
    // qDebug()<< Q_FUNC_INFO << m_hybridCommand;
    // show feedback on ERP speller for 500 ms
    correct = m_FirstSpeller->Correct();
    m_FirstSpeller->hide();
    m_SecondSpeller->show();
    m_SecondSpeller->showFeedback(m_hybridCommand, correct);

    // check if correct feedback, send it to robot, otherwise
    // repeat until feedback is correct in COPY MODE
    if (m_hybridStimulation->externalComm() == external_comm::ENABLED)
    {
        QString directionFeedback, directionDesired;
        // Send and Recieve feedback to/from Robot if external communication is enabled

        if(m_hybridStimulation->experimentMode() == operation_mode::COPY_MODE)
        {
            // direction controlled by 1st ERP
            directionFeedback = m_FirstFeedback[m_FirstFeedback.length() - 1];
            directionDesired  = m_FirstSpeller->getDesiredPhrase();

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

    hybridPostTrialEnd();
}

void HybridERP::hybridPostTrialEnd()
{
    //  qDebug()<< Q_FUNC_INFO;

    ++m_currentTrial;

    m_hybridState = trial_state::PRE_TRIAL;

    // More trials to conduct
    if( (m_currentTrial < m_trials) || (m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE))
    {
        if(m_FirstFeedback[m_FirstFeedback.length() - 1] == '5' && m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE) // stop command in ERP
        {
            qDebug()<< "Terminate FREE experiment";
            terminateExperiment();
        }
        else
        {
            m_hybridCommand = "";
            startTrial();
        }
    }

    // Experiment Ends
    else
    {
        if(m_hybridStimulation->experimentMode() == operation_mode::COPY_MODE)
        {
            m_FirstCorrect   = (m_FirstSpeller->m_correct / m_trials) * 100;
            m_SecondCorrect  = (m_SecondSpeller->m_correct / m_trials) * 100;
            qDebug()<< "Accuracy in ERP: " << m_FirstCorrect;
            qDebug()<< "Accuracy in SSVEP: "<< m_SecondCorrect;
        }

        terminateExperiment();
    }
}

void HybridERP::terminateExperiment()
{
    qDebug() << "Hybrid Experiment End";
    m_FirstSpeller->sendMarker(OVTK_StimulationId_ExperimentStop);
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
    m_FirstAnimation = new QPropertyAnimation(m_FirstSpeller, "windowOpacity");
    m_FirstAnimation->setDuration(750); //1000 //2000 //500 //250
    m_FirstAnimation->setStartValue(0.0); //
    m_FirstAnimation->setEndValue(1.0); //
    m_FirstAnimation->setEasingCurve(QEasingCurve::OutQuint);
    connect(m_FirstAnimation, SIGNAL(finished()), m_FirstSpeller, SLOT(startTrial()));

    m_SecondAnimation = new QPropertyAnimation(m_SecondSpeller, "windowOpacity");
    m_SecondAnimation->setDuration(750); //1000  //500 //250
    m_SecondAnimation->setStartValue(0.0); //1366
    m_SecondAnimation->setEndValue(1.0); // 0
    m_SecondAnimation->setEasingCurve(QEasingCurve::OutQuint);
    connect(m_SecondAnimation, SIGNAL(finished()), m_SecondSpeller, SLOT(startTrial()));
}

void HybridERP::initFirstSpeller(ERP *erp)
{
    int spellerType = erp->stimulationType();

    switch (spellerType)
    {
    case speller_type::FLASHING_SPELLER:
    {
        m_FirstSpeller = new FlashingSpeller();
        // m_ERPspeller->initSpeller(erp);
        break;
    }
    // case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
    case speller_type::FACES_SPELLER:
    case speller_type::INVERTED_FACE:
    case speller_type::COLORED_FACE:
    case speller_type::INVERTED_COLORED_FACE:
    {
        m_FirstSpeller = new FaceSpeller();
        //m_ERPspeller->initSpeller(erp);
        break;
    }
    // TODO: add the rest of Spellers
    }

}

void HybridERP::initSecondSpeller(ERP *erp)
{
    int spellerType = erp->stimulationType();

    switch (spellerType)
    {
    case speller_type::FLASHING_SPELLER:
    {
        m_SecondSpeller = new FlashingSpeller();
        // m_ERPspeller->initSpeller(erp);
        break;
    }
    // case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
    case speller_type::FACES_SPELLER:
    case speller_type::INVERTED_FACE:
    case speller_type::COLORED_FACE:
    case speller_type::INVERTED_COLORED_FACE:
    {
        m_SecondSpeller = new FaceSpeller();
        //m_ERPspeller->initSpeller(erp);
        break;
    }
    // TODO: add the rest of Spellers
    }

    m_SecondSpeller->hide();
}

HybridERP::~HybridERP(){}
