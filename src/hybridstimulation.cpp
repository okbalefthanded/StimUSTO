//
#include <QtDebug>
#include <QApplication>
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
}

HybridStimulation::HybridStimulation(Hybrid *hybridStimulation, Speller *ERPspeller, SsvepGL *ssvepGL)
{
    m_hybridStimulaiton = hybridStimulation;
    m_ERPspeller = ERPspeller;
    m_ssvepStimulation = ssvepGL;

    //    m_robotSocket = new QUdpSocket();
    //    m_robotSocket->bind(QHostAddress::LocalHost, m_robotPort); // provide Robot Address
    //    m_robotSocket->bind(QHostAddress("10.3.66.5"), m_robotPort);

    // connect to Robot
    m_robotSocket = new QTcpSocket();
    m_robotSocket->connectToHost(QHostAddress("10.3.66.5"), m_robotPort);
    if(m_robotSocket->waitForConnected())
    {
        qDebug() << "Robot Connection : State Connected";

    }
    else
    {
        qDebug() << "Robot Connection : State Not Connected";
    }

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
    else if(m_hybridStimulaiton->experimentMode() == operation_mode::COPY_MODE)
    {
        m_trials = m_hybridStimulaiton->m_ERPparadigm->desiredPhrase().count();
    }
    /*
    else if(m_hybridStimulation->experimentMode() == operation_mode::FREE_MODE)
    {

    }
    */

    connect(m_ERPspeller, SIGNAL(slotTerminated()), this, SLOT(switchState()) );
    connect(m_ssvepStimulation, SIGNAL(slotTerminated()), this, SLOT(switchState()));
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
        m_ssvepStimulation->m_flickeringSequence->sequence[0] = m_hybridStimulaiton->m_SSVEPparadigm->desiredPhrase()[m_currentTrial].digitValue();
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
    //    qDebug() << "[HYBRID TRIAL START]" << Q_FUNC_INFO;

    if(m_hybridState == trial_state::PRE_TRIAL)
    {
        hybridPreTrial();
    }

    if(m_hybridState == trial_state::STIMULUS)
    {

        if(m_switchStimulation)
        {
            qDebug() << Q_FUNC_INFO << "ERP trial";
            swichStimWindows();
            m_ERPspeller->startTrial();
        }

        else
        {
            qDebug() << Q_FUNC_INFO << "SSVEP trial";
            swichStimWindows();
            m_ssvepStimulation->startTrial();
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
    if(m_switchStimulation)
    {
        m_ssvepStimulation->hide();
        m_ERPspeller->show();
    }
    else
    {
        m_ssvepStimulation->setScreen(QGuiApplication::screens().last());
        m_ssvepStimulation->showFullScreen();
        m_ERPspeller->hide();
    }
}

void HybridStimulation::hybridPostTrial()
{
    qDebug() << "[HYBRID POST TRIAL]" << Q_FUNC_INFO;

    if(m_hybridStimulaiton->experimentMode() == operation_mode::COPY_MODE || m_hybridStimulaiton->experimentMode() == operation_mode::FREE_MODE)
    {
        m_ERPFeedback = m_ERPspeller->m_text;
        m_SSVEPFeedback += m_ssvepStimulation->m_sessionFeedback;
        qDebug() << Q_FUNC_INFO << "ERP feedback: " << m_ERPFeedback;
        qDebug() << Q_FUNC_INFO << "SSVEP feedback: " << m_SSVEPFeedback;
    }

    // Send and Recieve feedback to/from Robot
    qDebug() << "Sending Feedback to Robot";
    m_hybridCommand = m_ERPFeedback[m_currentTrial] + m_SSVEPFeedback.at(m_currentTrial);

    // m_hybridCommand = "12";
    if (!m_robotSocket->isOpen())
    {
        qDebug()<< "Not sending Feedback to Robot : Cannot send feedback socket is not open";
    }

    try

    {

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

    qDebug() << "Recieve State from Robot";
    m_robotSocket->waitForReadyRead();

    QByteArray robotState = m_robotSocket->readAll();

    quint8 rState = robotState.toUInt();
    qDebug()<< Q_FUNC_INFO << "Robot State recieved " << rState;
    if(rState == robot_state::READY)
    {
        qDebug()<< "Correct State";
        m_hybridState = trial_state::PRE_TRIAL;
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


    //
    ++m_currentTrial;
    m_hybridState = trial_state::PRE_TRIAL;

    if(m_currentTrial < m_trials || m_hybridStimulaiton->experimentMode() == operation_mode::FREE_MODE)
    {
        startTrial();
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

        qDebug() << "Hybrid Experiment End";
        //        m_ERPspeller->close();
        //        m_ssvepStimulation->close();
        //        emit experimentEnd();
    }
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
}

void HybridStimulation::initSSVEP(SSVEP *ssvep)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
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
    m_ssvepStimulation->hide();
}

HybridStimulation::~HybridStimulation()
{}
