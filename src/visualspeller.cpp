#include <QGridLayout>
#include <functional>
#include <QFileInfo>
#include <QWindow>
#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QMap>
#include <QDir>
//
#include "ovtk_stimulations.h"
#include "visualspeller.h"
#include "stimulation.h"
#include "logger.h"
#include "utils.h"

// VisualSpeller::VisualSpeller(QWidget *parent) : QWidget(parent), Stimulation()
VisualSpeller::VisualSpeller(ERP *t_erp) : Stimulation()
{
    // qDebug()<< Q_FUNC_INFO;
    setParadigm(t_erp);
    createLayout();
    endInit();
    // create a stimulation method also ??
    showWindow();
}

void VisualSpeller::preTrial()
{
    // qDebug()<<Q_FUNC_INFO <<"speller type" <<m_settings->stimulationType();

    if(m_trials == 0)
    {
        experimentStart();
        // initLogger();
    }

    startPreTrial();

    m_preTrialTimer->start();
    ++m_preTrialCount;

    endPreTrial();
}

void VisualSpeller::startTrial()
{
    // qDebug()<< Q_FUNC_INFO << m_state;

    // qDebug()<< Q_FUNC_INFO << m_settings->stimulationType() << m_settings->flashingMode();

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

void VisualSpeller::postTrial()
{
    // qDebug()<< Q_FUNC_INFO;

    ++m_trials;

    utils::wait(250); //

    if (m_text[m_text.length()-1] != "#")
    {
        if (m_settings->experimentMode() == operation_mode::COPY_MODE ||
            m_settings->experimentMode() == operation_mode::FREE_MODE)
        {
            // refreshTarget();

        }
        else if(m_settings->experimentMode() == operation_mode::CALIBRATION)
        {
            refreshTarget();
        }
    }
    //
    // Send and Recieve feedback to/from Robot if external communication is enabled
    // externalCommunication();
    if(m_settings->externalComm() == external_comm::ENABLED)
    {
        m_externComm->communicate(QString(m_text[m_text.length()-1]));
    }

    utils::wait(2000); // 2 sec pause before starting a new Trial // TODO

    postTrialEnd();
}

void VisualSpeller::startFlashing()
{
    // QString functionInfo = Q_FUNC_INFO;
    // qDebug()<< Q_FUNC_INFO << m_state;

    // log->write(functionInfo + " [PRE_SENDINFO] " + QTime::currentTime().toString("hh:mm:ss.zzz"));
    sendStimulationInfo();
    // log->write(functionInfo + "[POST_SENDINFO] " + QTime::currentTime().toString("hh:mm:ss.zzz"));
    m_stimMethodMap[m_settings->stimulationType()]();

    // log->write(functionInfo + "[PRE_SWITCH] " + QTime::currentTime().toString("hh:mm:ss.zzz"));
    switchStimulationTimers();
    // log->write(functionInfo + "[POST_SWITCH] " + QTime::currentTime().toString("hh:mm:ss.zzz"));
}

void VisualSpeller::pauseFlashing()
{
    // qDebug()<< Q_FUNC_INFO << m_state;
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    m_pauseMethodMap[m_settings->stimulationType()]();

    switchStimulationTimers();

    ++m_currentStimulation;

    trialEnd();
}

void VisualSpeller::feedback()
{
    receiveFeedback();

    m_textRow->setText(m_text);

    if (m_text[m_text.length()-1] != "#")
    {
        if (m_settings->experimentMode() == operation_mode::COPY_MODE)
        {
            if( m_text[m_text.length()-1] == m_desiredPhrase[m_currentLetter - 1])
            {
                m_speller->layout()->itemAt(m_currentTarget)->
                    widget()->setStyleSheet("QLabel { color : green; font: 40pt }");

                m_correct = true;
                ++m_correct;
            }
            else
            {
                m_speller->layout()->itemAt(m_currentTarget)->
                    widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");

                isCorrect = false;
            }
        }

        else if (m_settings->experimentMode() == operation_mode::FREE_MODE)
        {
            m_speller->layout()->itemAt(0)->
                widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");
        }
    }

    postTrial();
}
//
void VisualSpeller::createLayout()
{
    m_speller = new QWidget();
    m_speller->setProperty("windowTitle", "ERP Speller");

    QGridLayout *layout = nullptr;

    if(m_settings->language() == speller_language::ENGLISH)
    {
        layout = createLayoutEnglish();
    }

    else if(m_settings->language() == speller_language::ARABIC)
    {
        layout = createLayoutArabic();
    }

    m_speller->setLayout(layout);

    if (m_settings->flashingMode() == flashing_mode::SC)
    {
        m_nrElements = m_rows * m_cols;
    }
    else
    {
        m_nrElements = m_rows + m_cols;
    }

}

QGridLayout *VisualSpeller::createLayoutEnglish()
{
    QGridLayout *layout = prepareLayout();
    QString spellerStyle   = "background-color:" + m_settings->backgroundColor().toString() + ";";
    QString characterStyle = "font: " + m_settings->charFont().toString() + "pt; color: " + m_settings->charColor().toString() + ";";
    m_speller->setStyleSheet(spellerStyle);

    int label_h, label_w;

    // add speller ellements
    for(int i=1; i<m_rows+1; i++)
    {
        for(int j=0; j<m_cols; j++)
        {
            QLabel *element = new QLabel(m_speller);
            label_h = element->height();
            label_w = element->width();
            element->setText(utils::letters[i-1][j]);
            element->setStyleSheet(characterStyle);
            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element, i, j);
            m_presentedLetters.append(utils::letters[i-1][j]);
        }
    }

    return layout;
}

QGridLayout *VisualSpeller::createLayoutArabic()
{
    QGridLayout *layout = prepareLayout();
    QString spellerStyle   = "background-color:" + m_settings->backgroundColor().toString() + ";";
    QString characterStyle = "font: " + m_settings->charFont().toString() + "pt; color: " + m_settings->charColor().toString() + ";";
    m_speller->setStyleSheet(spellerStyle);

    int k = 0, nbr=0;
    int label_h, label_w;

    // add speller ellements
    for(int i=1; i<m_rows+1; i++)
    {
        for(int j=m_cols; j>0; --j)
        {
            QLabel *element = new QLabel(m_speller);
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

            element->setStyleSheet(characterStyle);
            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element, i, j);

            k++;
        }
    }

    return layout;
}

QGridLayout *VisualSpeller::prepareLayout()
{
    // m_rows = 6, m_cols = 6;

    QGridLayout *layout = new QGridLayout();

    m_rows = m_settings->rows();
    m_cols = m_settings->cols();

    m_textRow = new QLabel(m_speller);
    m_textRow->setText(m_desiredPhrase);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignLeft);
    //    textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_textRow, 0, 0, 1, 0);

    return layout;
}

void VisualSpeller::refreshLayout()
{
    // TODO
}

// stimulation loop helpers: start/end for pre/post trial methods
void VisualSpeller::experimentStart()
{
    sendMarker(OVTK_StimulationId_ExperimentStart);
    // setting a pre-trail duration longer for calibration phase
    // than Copy_mode/Free mode phases
    // if (m_ERP->experimentMode() == operation_mode::CALIBRATION)
    if (m_settings->experimentMode() == operation_mode::CALIBRATION)
    {
        m_preTrialTimer->setInterval(1000);
    }
    else
    {
        m_preTrialTimer->setInterval(500);
    }
}

void VisualSpeller::startPreTrial()
{
    // qDebug()<< Q_FUNC_INFO;
    if (m_preTrialCount == 0)
    {
        sendMarker(OVTK_StimulationId_TrialStart);
        // m_flashingSequence = new RandomFlashSequence(getnElements(), m_settings->nrSequences());
        m_flashingSequence = new RandomFlashSequence(getnElements(),
                                                     m_settings->nrSequences(),
                                                     m_settings->flashingMode(),
                                                     m_rows,
                                                     m_cols);


        /*
        for(int i=0; i< m_flashingSequence->sequence.length(); i++)
        {
            qDebug()<< Q_FUNC_INFO << m_flashingSequence->sequence[i];
            qDebug()<< Q_FUNC_INFO << m_flashingSequence->sequenceSet.at(i);
        }
        */

        if (m_settings->experimentMode() == operation_mode::CALIBRATION)
        {
            highlightTarget();
            m_text += m_desiredPhrase[m_currentLetter];
            m_textRow->setText(m_text);
        }
        else if(m_settings->experimentMode() == operation_mode::COPY_MODE)
        {
            highlightTarget();
        }
        else if(m_settings->experimentMode() == operation_mode::FREE_MODE)
        {
            utils::wait(500);
        }
    }
    else return;
}

void VisualSpeller::endPreTrial()
{
    if (m_preTrialCount > m_preTrialWait || m_settings->experimentMode() == operation_mode::FREE_MODE)
    {
        if(m_settings->experimentMode() == operation_mode::COPY_MODE ||
            m_settings->experimentMode() == operation_mode::CALIBRATION)
        //if( m_settings->experimentMode() == operation_mode::COPY_MODE)
        {
            refreshTarget();
        }
        m_preTrialTimer->stop();
        m_preTrialCount = 0;
        m_state = trial_state::STIMULUS;
    }

    else return;
}

void VisualSpeller::trialEnd()
{
    // qDebug() << Q_FUNC_INFO << m_currentStimulation << m_flashingSequence->sequence.count();

    if (m_currentStimulation >= m_flashingSequence->sequence.count())
    {
        ++m_currentLetter;
        m_isiTimer->stop();
        m_stimTimer->stop();

        // utils::wait(1000); // time window after last epoch/stim
        utils::wait(500);
        // utils::wait(700); // 700 ms == epoch time windows

        sendMarker(OVTK_StimulationId_TrialStop);
        m_state = trial_state::FEEDBACK;

        if(m_settings->experimentMode() == operation_mode::COPY_MODE || m_settings->experimentMode() == operation_mode::FREE_MODE)
        {
            feedback();
        }
        else if(m_settings->experimentMode() == operation_mode::CALIBRATION)
        {
            postTrial();
        }
    }
}

void VisualSpeller::postTrialEnd()
{
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // utils::wait(500 - m_delay);// 500 // 1000
    utils::wait(500);// 1000 // make this for single ERP only
    //
    // sendMarker(OVTK_StimulationId_SegmentStart);
    m_currentStimulation = 0;
    m_state = trial_state::PRE_TRIAL;
    //
    if (m_currentLetter >= m_desiredPhrase.length() &&
        m_desiredPhrase.length() != 1 &&
        (m_settings->experimentMode() == operation_mode::COPY_MODE ||
         m_settings->experimentMode() == operation_mode::CALIBRATION)
        )
    {
        m_correct = (m_correct / m_desiredPhrase.length()) * 100;
        qDebug()<< "Accuracy on ERP session: " << m_correct;
        qDebug()<< "Experiment End, closing speller";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        if(m_settings->externalComm() == external_comm::ENABLED)
        {
            m_externComm->communicate("00");
        }
        utils::wait(2000);
        // emit(slotTerminated());
        // this->close();
    }
    else if(m_desiredPhrase.length() <= 1)
    {
        m_currentLetter = 0;
        // test if correct
        //qDebug()<< m_desiredPhrase << " "<<m_text[m_text.length()-1];
        if (m_desiredPhrase != m_text[m_text.length()-1])
        {
            //
            startTrial();
        }
        else
        {
            sendMarker(OVTK_StimulationId_RestStart);
            emit(slotTerminated());
            return;
        }
    }
    else
    {
        startTrial();
    }

}

void VisualSpeller::sendStimulationInfo()
{

    sendMarker(OVTK_StimulationId_VisualStimulationStart);
    sendMarker(config::OVTK_StimulationLabel_Base + m_flashingSequence->sequence[m_currentStimulation]);

    // send target marker
    if (m_settings->experimentMode() == operation_mode::CALIBRATION ||
        m_settings->experimentMode() == operation_mode::COPY_MODE)
    {
        if (isTarget())
        {
            sendMarker(OVTK_StimulationId_Target);
        }
        else
        {
            sendMarker(OVTK_StimulationId_NonTarget);
        }
    }
}

void VisualSpeller::switchStimulationTimers()
{
    if(m_state == trial_state::STIMULUS)
    {
        // qDebug()<< Q_FUNC_INFO << "[_STIMTIMER] "<< QTime::currentTime();
        m_stimTimer->start();
        // qDebug()<< Q_FUNC_INFO << "[post_STIMTIMER] "<< QTime::currentTime();

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


// graphics manipulations
void VisualSpeller::stimulationFlash()
{
    int start, end = 0;

    start = QTime::currentTime().msec();
    // QString functionInfo = Q_FUNC_INFO;
    // log->write(functionInfo + " [PRE_STIM] " + QTime::currentTime().toString("hh:mm:ss.zzz"));

    m_layout = m_speller->layout();
    const QVector<int>& currentSequence = m_flashingSequence->sequenceSet.at(m_currentStimulation);
    int sequenceLength = currentSequence.length();
    int index = 0;

    for (int i = 0; i < sequenceLength; ++i)
    {
        index = currentSequence.at(i);
        m_item = m_layout->itemAt(index);
        m_item->widget()->setStyleSheet("QLabel { color : white; font: 60pt }");
        QCoreApplication::processEvents();
    }

    // log->write(functionInfo + " [POST_STIM] " + QTime::currentTime().toString("hh:mm:ss.zzz"));
    end = QTime::currentTime().msec();
    updateStimTimer(start, end);
}

void VisualSpeller::stimulationPixMap()
{
    // qDebug()<< Q_FUNC_INFO;
    int start, end = 0;

    start = QTime::currentTime().msec();
    m_layout = m_speller->layout();
    const QVector<int>& currentSequence = m_flashingSequence->sequenceSet.at(m_currentStimulation);
    int sequenceLength = currentSequence.length();
    int index = 0;

    for (int i = 0; i < sequenceLength; ++i)
    {
        index = currentSequence.at(i);
        m_item = m_layout->itemAt(index);
        m_item->widget()->setProperty("pixmap", *m_stimulus);

        QCoreApplication::processEvents();
    }
    end = QTime::currentTime().msec();
    //
   updateStimTimer(start, end);
}

void VisualSpeller::stimulationMultiPixMap()
{
    int start, end = 0;

    start = QTime::currentTime().msec();

    if(m_settings->flashingMode() == flashing_mode::SC)
    {
        QPixmap pixmap;
        int currentStim = m_flashingSequence->sequence[m_currentStimulation];
        int row = utils::indexToGridCoordinates(currentStim, m_rows);
        int n_stimuli = m_settings->stimuliFilePaths().length();
        int step = row % n_stimuli;
        pixmap = m_multStimuli[step];
        m_speller
            ->layout()
            ->itemAt(currentStim)
            ->widget()
            ->setProperty("pixmap", pixmap);
    }

    else
    {
        m_layout = m_speller->layout();
        const QVector<int>& currentSequence = m_flashingSequence->sequenceSet.at(m_currentStimulation);
        int sequenceLength = currentSequence.length();

        for (int i = 0; i < sequenceLength; ++i)
        {
            m_layout->itemAt(currentSequence.at(i))->widget()->setProperty("pixmap", m_multStimuli[i]);
            QCoreApplication::processEvents();
        }
    }

    end = QTime::currentTime().msec();
    //
    updateStimTimer(start, end);
}

void VisualSpeller::pauseFlash()
{
    int start, end = 0;

    start = QTime::currentTime().msec();
    QString functionInfo = Q_FUNC_INFO;
    // log->write(functionInfo + " [PRE_PAUSE] " + QTime::currentTime().toString("hh:mm:ss.zzz"));

    m_layout = m_speller->layout();
    const QVector<int>& currentSequence = m_flashingSequence->sequenceSet.at(m_currentStimulation);
    int sequenceLength = currentSequence.length();
    int index = 0;


    for (int i = 0; i < sequenceLength; ++i)
    {
        index = currentSequence.at(i);
        m_item = m_layout->itemAt(index);
        m_item->widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
        QCoreApplication::processEvents();
    }

    end = QTime::currentTime().msec();
    //
    updateIsiTimer(start, end);

    // log->write(functionInfo + " [POST_PAUSE] " + QTime::currentTime().toString("hh:mm:ss.zzz"));
}

void VisualSpeller::pauseStimPixMap()
{
    int start, end = 0;

    start = QTime::currentTime().msec();

    for(int i=0; i< m_flashingSequence->sequenceSet.at(m_currentStimulation).length();i++)
    {
        m_speller
            ->layout()
            ->itemAt(m_flashingSequence->sequenceSet.at(m_currentStimulation).at(i))
            ->widget()
            ->setProperty("text", m_presentedLetters[m_flashingSequence->sequenceSet.at(m_currentStimulation).at(i)-1]);
    }

    end = QTime::currentTime().msec();
    //
    updateIsiTimer(start, end);
}

void VisualSpeller::highlightTarget()
{
    int currentTarget = getCurrentTarget();

    m_speller
        ->layout()
        ->itemAt(currentTarget)
        ->widget()->
        setStyleSheet("QLabel { color : red; font: 60pt }");

}

void VisualSpeller::refreshTarget()
{
    m_speller
        ->layout()
        ->itemAt(m_currentTarget)
        ->widget()->
        setStyleSheet("QLabel { color : gray; font: 40pt }");
}

// general helpers
bool VisualSpeller::isTarget()
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

int VisualSpeller::getnElements()
{
    return m_nrElements;
}

int VisualSpeller::getCurrentTarget()
{
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
    return m_currentTarget;
}

void VisualSpeller::setParadigm(ERP *erp)
{
    m_settings = erp;
    // setTimers(m_settings->stimulationDuration(), m_settings->breakDuration());
    setDesiredPhrase(m_settings->desiredPhrase());

    // m_textRow->setText(m_desiredPhrase);
    // external comm
    // a temporary hack
    // qDebug()<< "Lets see external comm" <<m_ERP->externalComm();

    if(m_settings->externalComm() == external_comm::ENABLED)
    {
        m_externComm = new ExternComm(m_settings->externalAddress(), 12347, m_settings->externalComm());
    }
}

void VisualSpeller::setDesiredPhrase(const QString &t_desiredPhrase)
{
    m_desiredPhrase = t_desiredPhrase;
}

void VisualSpeller::setTimers(int t_stimulation, int t_isi)
{
    m_stimTimer->setTimerType(Qt::PreciseTimer);
    m_stimTimer->setSingleShot(true);
    m_stimTimer->setInterval(t_stimulation);

    m_isiTimer->setTimerType(Qt::PreciseTimer);
    m_isiTimer->setSingleShot(true);
    m_isiTimer->setInterval(t_isi);
}

void VisualSpeller::updateStimTimer(int start, int end)
{
    m_guiDelay = end - start;
    m_stimTimer->setInterval(m_settings->stimulationDuration() - m_guiDelay);
}

void VisualSpeller::updateIsiTimer(int start, int end)
{
    m_guiDelay = end - start;
    m_isiTimer->setInterval(m_settings->breakDuration() - m_guiDelay);
}
//

// Initializers
void VisualSpeller::endInit()
{
    // qDebug()<< Q_FUNC_INFO;
    initMethodMap();
    initTimers();
    initFeedbackSocket();
    initStimuli();
    m_state = trial_state::PRE_TRIAL;

    // init logger
    // QDir logsDir(QCoreApplication::applicationDirPath() + "/logs");
    // if(!logsDir.exists())
    // {
    //     logsDir.mkdir(logsDir.path());
    // }
    // QString fileName;

    // fileName =  logsDir.filePath("erp_test" +QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss")+".txt");
    // log = new Logger(this, fileName);

}

void VisualSpeller::initMethodMap()
{
    // Stimulation Methods
    m_stimMethodMap[speller_type::FLASH]    = std::bind(&VisualSpeller::stimulationFlash, this);
    m_stimMethodMap[speller_type::SINGLE]   = std::bind(&VisualSpeller::stimulationPixMap, this);
    m_stimMethodMap[speller_type::MULTIPLE] = std::bind(&VisualSpeller::stimulationMultiPixMap, this);
    m_stimMethodMap[speller_type::MISMATCH] = std::bind(&VisualSpeller::stimulationMismatchFace, this);

    // Pause flashing Methods
    m_pauseMethodMap[speller_type::FLASH]    = std::bind(&VisualSpeller::pauseFlash, this);
    m_pauseMethodMap[speller_type::SINGLE]   = std::bind(&VisualSpeller::pauseStimPixMap, this);
    m_pauseMethodMap[speller_type::MULTIPLE] = std::bind(&VisualSpeller::pauseStimPixMap, this);
    m_pauseMethodMap[speller_type::MISMATCH] = std::bind(&VisualSpeller::pauseStimPixMap, this);
}

void VisualSpeller::initTimers()
{
    // qDebug()<< Q_FUNC_INFO;

    m_stimTimer     = new QTimer(this);
    m_isiTimer      = new QTimer(this);
    m_preTrialTimer = new QTimer(this);

    m_stimTimer->setTimerType(Qt::PreciseTimer);
    m_stimTimer->setSingleShot(true);
    m_stimTimer->setInterval(m_settings->stimulationDuration()); //default value

    m_isiTimer->setTimerType(Qt::PreciseTimer);
    m_isiTimer->setSingleShot(true);
    m_isiTimer->setInterval(m_settings->breakDuration()); //default value

    m_preTrialTimer->setTimerType(Qt::PreciseTimer);
    m_preTrialTimer->setInterval(1000);
    m_preTrialTimer->setSingleShot(true);

    connect(m_stimTimer,     SIGNAL(timeout()), this, SLOT(pauseFlashing()), Qt::DirectConnection);
    connect(m_isiTimer,      SIGNAL(timeout()), this, SLOT(startFlashing()), Qt::DirectConnection);
    connect(m_preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );
}

void VisualSpeller::initStimuli()
{
    QString stimulusPath;
    int n_stimuli = qMax(m_rows, m_cols);
    int min       = qMin(qMin(m_rows, m_cols), m_settings->stimuliFilePaths().length());
    int k = 0;
    m_multStimuli.reserve(n_stimuli);

    // resize pictures of stimuli to a given size, here 130x130 px
    for(int i=0; i<n_stimuli; i++)
    {
        if (i >= min) k = i % min;
        stimulusPath = m_settings->stimuliFilePaths().at(k).toString();
        m_multStimuli.append(QPixmap(stimulusPath).scaled(130, 130, Qt::KeepAspectRatio));
        ++k;
    }

    m_stimulus = &m_multStimuli[0]; // for a single stimulus
}


// general utils
void VisualSpeller::showWindow()
{
    m_speller->show();

    if(qApp->screens().count() == 2)
    {
        m_speller->windowHandle()->setScreen(qApp->screens().last());
        m_speller->showFullScreen();
    }
    else
    {
        // this->showMaximized();
        m_speller->showFullScreen();
    }

    m_speller->setStyleSheet("background-color : black");
}


// destructor
VisualSpeller::~VisualSpeller(){}

