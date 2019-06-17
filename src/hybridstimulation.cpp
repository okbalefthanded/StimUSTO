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

    m_ssvepStimulation->m_firstRun = false;
    m_ssvepStimulation->m_flickeringSequence = new RandomFlashSequence(1, 1);
    RandomFlashSequence *rfseq = new RandomFlashSequence(m_hybridStimulaiton->m_SSVEPparadigm->nrElements(),
                                                         m_hybridStimulaiton->m_SSVEPparadigm->nrSequences() / m_hybridStimulaiton->m_SSVEPparadigm->nrElements());
    m_hybridStimulaiton->m_SSVEPparadigm->setDesiredPhrase(rfseq->toString());

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
