#include <QTimer>
//
#include "openglstimulation.h"
#include "ssvepstimulation_.h"
#include "ovtk_stimulations.h"
#include "glutils.h"

SSVEPStimulation::SSVEPStimulation(SSVEP *t_ssvep)
{
    setParadigm(t_ssvep);
    // create SSVEP OpenGLStimulation
    createLayout();
    // init the rest
    initTimers();
    initFeedbackSocket();
    m_state = trial_state::PRE_TRIAL;
}

// Stimulation Loop
void SSVEPStimulation::startTrial()
{
    if (m_state == trial_state::PRE_TRIAL)
    {
        preTrial();
    }
    if (m_state == trial_state::STIMULUS)
    {
        Flickering();
    }
    if (m_state == trial_state::POST_TRIAL)
    {
        postTrial();
    }
}

void SSVEPStimulation::Flickering()
{
    // send markers
    if(m_ui->m_index == 0)
    {
        sendMarker(config::OVTK_StimulationLabel_Base + m_flickeringSequence->sequence[m_currentFlicker]);
        sendMarker(OVTK_StimulationId_VisualSteadyStateStimulationStart);
    }


    while(m_ui->m_index <= m_ui->m_flicker[0].size())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (m_receivedFeedback)
        {
            break;
        }
    }


    disconnect(m_ui, SIGNAL(frameSwapped()), m_ui, SLOT(update()));
    // elapsedMs = correctortimer->elapsed();
    m_ui->refreshFlickers();
    // hack
    //
    sendMarker(OVTK_StimulationId_TrialStop);
    m_state = trial_state::POST_TRIAL;
}

void SSVEPStimulation::preTrial()
{
    m_receivedFeedback = false;

    if(m_trials == 0)
    {
        sendMarker(OVTK_StimulationId_ExperimentStart);
    }

    if(m_firstRun)
    {
        if(m_settings->desiredPhrase().isEmpty())
        {
            m_flickeringSequence = new RandomFlashSequence(m_settings->nrElements(), m_settings->nrSequences() / m_settings->nrElements());
        }

        else
        {
            m_flickeringSequence = new RandomFlashSequence();
            m_flickeringSequence->sequence = RandomFlashSequence::toSequence(m_settings->desiredPhrase());
        }

        m_firstRun = false;
    }

    if (m_preTrialCount == 1)
    {
        sendMarker(OVTK_StimulationId_TrialStart);

        if (m_settings->experimentMode() == operation_mode::CALIBRATION ||
            m_settings->experimentMode() == operation_mode::COPY_MODE)
        {
            m_ui->highlightTarget();
        }
    }

    m_preTrialTimer->start();
    m_preTrialCount++;

    if (m_preTrialCount > m_preTrialWait)
    {
        m_ui->scheduleRedraw();
        m_preTrialTimer->stop();
        m_preTrialCount = 1;
        m_state = trial_state::STIMULUS;

        if(m_ui->m_index == 0)
        {
            connect(m_ui, SIGNAL(frameSwapped()), m_ui, SLOT(update()));
        }
    }
}

void SSVEPStimulation::postTrial()
{
    // initElements();
    // qDebug()<< Q_FUNC_INFO;
    // disconnect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    // refreshCircles(); // initElements();

    m_ui->m_index = 0;
    //  m_state = trial_state::PRE_TRIAL;

    // feedback
    if(m_settings->experimentMode() == operation_mode::COPY_MODE ||
        m_settings->experimentMode() == operation_mode::FREE_MODE)
    {
        feedback();  // wait for feedback

        if (m_presentFeedback)
        {
            // feedback for 0.5 sec & refresh
            // utils::wait(300);
            utils::wait(500);
            m_ui->refresh(m_sessionFeedback[m_currentFlicker].digitValue()-1);
        }
    }

    // calibration mode
    else
    {
        m_ui->refreshTarget();
    }

    m_state = trial_state::PRE_TRIAL;
    // refreshCircles();

    // externalCommunication();

    if(m_settings->externalComm() == external_comm::ENABLED)
    {
        // std::string cmd = QString(m_sessionFeedback[m_currentFlicker]).toStdString();
        m_externComm->communicate(QString(m_sessionFeedback[m_currentFlicker]));
    }

    postTrialEnd();
}

void SSVEPStimulation::feedback()
{
    // receiveFeedback
    if (!m_receivedFeedback)
    {
        m_feedbackSocket->waitForReadyRead();
    }

    if(m_presentFeedback)
    {
        if(m_settings->experimentMode() == operation_mode::COPY_MODE)
        {
            if(m_sessionFeedback[m_currentFlicker].digitValue() == m_flickeringSequence->sequence[m_currentFlicker])
            {
                m_ui->highlightFeedback(glColors::green, m_flickeringSequence->sequence[m_currentFlicker]-1);
                ++m_correct;
            }
            else
            {
                m_ui->highlightFeedback(glColors::blue, m_sessionFeedback[m_currentFlicker].digitValue()-1);
            }
        }
        else if(m_settings->experimentMode() == operation_mode::FREE_MODE)
        {
            m_ui->highlightFeedback(glColors::red, m_sessionFeedback[m_currentFlicker].digitValue()-1);
        }
    }

}

void SSVEPStimulation::postTrialEnd()
{
    // qDebug()<< Q_FUNC_INFO << m_flickeringSequence->sequence.length();
    sendMarker(OVTK_StimulationId_SegmentStart);

    ++m_currentFlicker;
    ++m_trials;

    if (m_currentFlicker < m_flickeringSequence->sequence.size() &&
        m_flickeringSequence->sequence.length() != 1 &&
        (m_settings->experimentMode() == operation_mode::COPY_MODE ||
         m_settings->experimentMode() == operation_mode::CALIBRATION ||
         m_settings->experimentMode() == operation_mode::FREE_MODE))
    {
        startTrial();
    }
    else if (m_flickeringSequence->sequence.length() <= 1)
    {
        m_currentFlicker = 0;
        emit(slotTerminated());
        return;
    }
    else
    {
        m_correct = (m_correct / m_flickeringSequence->sequence.size()) * 100;
        qDebug()<< "Accuracy in SSVEP session: " << m_correct;
        qDebug()<< "Experiment End, closing SSVEP stimulation";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        if(m_settings->externalComm() == external_comm::ENABLED)
        {
            m_externComm->communicate("00");
        }
        utils::wait(2000);
    }
}

// Graphics manipulation
void SSVEPStimulation::createLayout()
{
    m_ui = new OpenGLStimulation(m_settings);
    m_ui->initElements();
    //
}

// inits
void SSVEPStimulation::initTimers()
{
    m_preTrialWait = 3;

    m_preTrialTimer = new QTimer(this);
    m_preTrialTimer->setTimerType(Qt::PreciseTimer);
    m_preTrialTimer->setInterval(m_settings->breakDuration() / 2);
    m_preTrialTimer->setSingleShot(true);
    connect(m_preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );
}

// Getters and Setters
void SSVEPStimulation::setParadigm(SSVEP *t_erp)
{
    m_settings = t_erp;
}

// Destructor
SSVEPStimulation::~SSVEPStimulation(){}


