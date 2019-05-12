//
#include <QtDebug>
//
#include "hybridstimulation.h"
#include "facespeller.h"
#include "flashingspeller.h"
#include "hybrid.h"
#include "ovtk_stimulations.h"
//
// HybridStimulation::HybridStimulation(ERP *erp, SSVEP *ssvep)
HybridStimulation::HybridStimulation(Hybrid *hybridStimulation)
{
    // initERPspeller(erp);
    // initSSVEP(ssvep);
    m_hybridStimulaiton = new Hybrid();
    initERPspeller(hybridStimulation->m_ERPparadigm);
    initSSVEP(hybridStimulation->m_SSVEPparadigm);
}

HybridStimulation::HybridStimulation(Hybrid *hybridStimulation, Speller *ERPspeller, SsvepGL *ssvepGL)
{
    m_hybridStimulaiton = hybridStimulation;
    m_ERPspeller = ERPspeller;
    m_ssvepStimulation = ssvepGL;
    //
    m_ssvepStimulation->m_firstRun = false;
    m_ssvepStimulation->m_flickeringSequence = new RandomFlashSequence(1, 1);
    RandomFlashSequence *rfseq = new RandomFlashSequence(m_ssvepStimulation->m_nrElements, m_ssvepStimulation->m_stimulationSequence / m_ssvepStimulation->m_nrElements);
    m_hybridStimulaiton->m_SSVEPparadigm->setDesiredPhrase(rfseq->toString());
    //
    m_trials = m_hybridStimulaiton->m_ERPparadigm->desiredPhrase().count();
    qDebug()<< Q_FUNC_INFO << "nr trials: " << m_trials;
    qDebug()<< Q_FUNC_INFO << "erp desired phrase" << m_hybridStimulaiton->m_ERPparadigm->desiredPhrase();
    //
    connect(m_ERPspeller, SIGNAL(slotTerminated()), this, SLOT(switchState()) );
    connect(m_ssvepStimulation, SIGNAL(slotTerminated()), this, SLOT(switchState()));
}

void HybridStimulation::hybridPreTrial()
{
    qDebug() << "[HYBRID PRETRIAL START]" << Q_FUNC_INFO;

    if(m_hybridStimulaiton->experimentMode() == operation_mode::CALIBRATION ||
            m_hybridStimulaiton->experimentMode() == operation_mode::COPY_MODE)
    {
        qDebug() << "current trial" << m_currentTrial;
        qDebug() << "current trial" <<m_hybridStimulaiton->m_ERPparadigm->desiredPhrase()[m_currentTrial];

        m_ERPspeller->setDesiredPhrase(m_hybridStimulaiton->m_ERPparadigm->desiredPhrase()[m_currentTrial].toLower());
        m_ssvepStimulation->m_flickeringSequence->sequence[0] = m_hybridStimulaiton->m_SSVEPparadigm->desiredPhrase()[m_currentTrial].digitValue();
    }
    // switchState();
    m_hybridState = trial_state::STIMULUS;
}

void HybridStimulation::startTrial()
{
    qDebug() << "[HYBRID TRIAL START]" << Q_FUNC_INFO;

    if(m_hybridState == trial_state::PRE_TRIAL)
    {
        hybridPreTrial();
    }

    if(m_hybridState == trial_state::STIMULUS)
    {

        if(m_switchStimulation)
        {
            qDebug() << Q_FUNC_INFO << "ERP trial switch state:" << m_switchStimulation;
            // m_ERPspeller->show();
            // m_ssvepStimulation->hide();
            swichStimWindows();
            m_ERPspeller->startTrial();
        }

        else
        {
            qDebug() << Q_FUNC_INFO << "SSVEP trial switch state:" << m_switchStimulation;
            //  m_ERPspeller->hide();
            //  m_ssvepStimulation->show();
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

    qDebug() << Q_FUNC_INFO;

    /* else if(m_hybridState == trial_state::STIMULUS)
        m_hybridState = trial_state::POST_TRIAL;

    else if(m_hybridState == trial_state::POST_TRIAL)
        m_hybridState = trial_state::PRE_TRIAL;
    */

    //   if(m_currentTrial >= m_trials)
    //   {
    //       m_hybridState = trial_state::POST_TRIAL;
    //   }
    // if(!m_switchStimulation)
    //     m_hybridState = trial_state::PRE_TRIAL;
    if(!m_switchStimulation)
        m_hybridState = trial_state::POST_TRIAL;
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
    // switchState();
    ++m_currentTrial;
    m_hybridState = trial_state::PRE_TRIAL;
    if(m_currentTrial <= m_trials)
    {
        startTrial();
    }
    else
    {
        qDebug() << "Experiment End";
    }
}



void HybridStimulation::initERPspeller(ERP *erp)
{
    int spellerType = erp->stimulationType();

    switch (spellerType)
    {
    case speller_type::FLASHING_SPELLER:
    {
        //FlashingSpeller *flashSpeller = new FlashingSpeller();
        //flashSpeller->initSpeller(erp);
        m_ERPspeller = new FlashingSpeller();
        m_ERPspeller->initSpeller(erp);
        break;
    }
        // case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
    case speller_type::FACES_SPELLER:
    case speller_type::INVERTED_FACE:
    case speller_type::COLORED_FACE:
    case speller_type::INVERTED_COLORED_FACE:
    {
        // FaceSpeller *faceSpeller = new FaceSpeller();
        // faceSpeller->initSpeller(erp);
        m_ERPspeller = new FaceSpeller();
        m_ERPspeller->initSpeller(erp);
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

    //SsvepGL *ssvepStimulation = new SsvepGL(*ssvep);
    m_ssvepStimulation = new SsvepGL(*ssvep);
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
