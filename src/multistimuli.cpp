#include <QLayout>
#include <QTimer>
#include <QTime>
#include <QMap>
//
#include "multistimuli.h"
#include "erp.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//
MultiStimuli::MultiStimuli(QWidget *parent) : Speller(0)
{
    // qDebug()<< Q_FUNC_INFO;

    setupUi(this);

    this->setProperty("windowTitle", "ERP MultiStim Speller");

    showWindow();

    createLayout();
    initTimers();
    initFeedbackSocket();

    m_state = trial_state::PRE_TRIAL;
}


void MultiStimuli::startFlashing()
{
    // qDebug() << Q_FUNC_INFO;
    sendStimulationInfo();
    // stimulation
    updateWidgets(m_stimuli);

    switchStimulationTimers();
}

void MultiStimuli::pauseFlashing()
{
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();
    // qDebug() << Q_FUNC_INFO;

    updateWidgets(m_icons);

    switchStimulationTimers();
    // ++m_currentStimulation;
    if(m_ERP->stimulationType() == speller_type::DUAL_STIM)
    {
        m_currentStimulation += 2;
    }
    else if(m_ERP->stimulationType() == speller_type::MULTI_STIM)
    {
        m_currentStimulation = m_currentStimulation + (m_nrElements / 3);
    }
    // m_currentStimulation = m_currentStimulation + (m_nrElements / 3);
    trialEnd();

}

void MultiStimuli::preTrial()
{
   //  qDebug()<< Q_FUNC_INFO;

    if(m_trials == 0)
    {
        sendMarker(OVTK_StimulationId_ExperimentStart);

        QMap<int, QString> t_stimMap;

        if(m_ERP->stimulationType() == speller_type::DUAL_STIM)
        {

            t_stimMap = m_stimuliMapDual;
        }
        else if(m_ERP->stimulationType() == speller_type::MULTI_STIM)
        {
            t_stimMap = m_stimuliMap;
        }

        // foreach(QString value, m_stimuliMap)
        foreach(QString value, t_stimMap)
        {
            QPixmap pic(":/images/"+value);

            m_stimuli.append(pic);
        }
    }

    if (m_preTrialCount == 0)
    {
        // Refresh previous feedback
        /*
        if(m_text.length() > 0)
        {

            if(m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                    m_ERP->experimentMode() == operation_mode::FREE_MODE)
            {
                int id = m_text[m_text.length()-1].digitValue();
                QPixmap map = m_icons[id-1];
                m_element = new QLabel();
                m_element->setPixmap(map);
                m_element->setAlignment(Qt::AlignCenter);

                this->layout()->replaceWidget(this->
                                              layout()->
                                              itemAt(id)->
                                              widget(),
                                              m_element,
                                              Qt::FindDirectChildrenOnly);
            }
        }
        */
        //
        sendMarker(OVTK_StimulationId_TrialStart);
        //m_flashingSequence = new RandomFlashSequence(m_nrElements / 3, m_ERP->nrSequences());


        if (m_ERP->stimulationType() == speller_type::MULTI_STIM)
        {
            m_flashingSequence = new RandomFlashSequence(m_nrElements, m_ERP->nrSequences(), 3, 3);
        }
        else if (m_ERP->stimulationType() == speller_type::DUAL_STIM)
        {
            m_flashingSequence = new RandomFlashSequence(m_ERP->nrSequences());
        }

        // qDebug() << Q_FUNC_INFO << m_flashingSequence->sequence;

        if (m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            highlightTarget();
            m_text += m_desiredPhrase[m_currentLetter];
            // m_textRow->setText(m_text);
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

void MultiStimuli::createLayout()
{
    // create special laytout
    qDebug()<< Q_FUNC_INFO;
    // speller settings
    m_rows = 3;
    m_cols = 3;
    m_nrElements = m_rows * m_cols;

    m_icons = QList<QPixmap>();
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);

    QGridLayout *layout = new QGridLayout();

    m_textRow = new QLabel(this);
    m_textRow->setStyleSheet("font:25pt; color:gray; border-color:white;");
    m_textRow->setGeometry(0, 0, 10, 10);
    qDebug()<< " textrow geometry "<< m_textRow->geometry();
    m_textRow->setAlignment(Qt::AlignLeft);
    m_textRow->setObjectName("Desired Phrase Row");
    // m_textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->setHorizontalSpacing(400);
    layout->setVerticalSpacing(100);
    // layout->addWidget(m_textRow, 0, 0, 1, 0);
    // layout->addWidget(m_textRow, 0, 0);

    m_textRow->setText(m_desiredPhrase);

    int k = 1;
    // bool splitter = false;
    QString stimName;
    QPixmap pic;
    QImage iconImage;
    int label_h, label_w;
    // add speller ellements
    for(int i=1; i<m_rows+1; i++)
        //for(int i=0; i<m_rows; i++)
    {
        //  splitter = false;

        for(int j=0; j<m_cols; j++)
        {
            QLabel *element = new QLabel(this);
            // label_h = element->height() + 40;
            //  label_w = element->width() + 40;
            label_h = element->height() + 20;
            label_w = element->width() + 20;

            /* if (splitter)
           {
                element->setText("#");
                element->setStyleSheet("font: 40pt; color:gray");
                element->setAlignment(Qt::AlignCenter);
                splitter = false;
            }
            else
            {*/
            stimName = ":/images/" + QString::number(k) + ".png"; // directions images
            pic = QPixmap(stimName);

            element->setPixmap(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));
            element->setAlignment(Qt::AlignCenter);
            m_icons.append(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));
            // splitter = true;

            //}

            layout->addWidget(element, i, j);
            m_presentedLetters.append(QString::number(k));
            k++;

        }
    }

    this->setLayout(layout);

}

void MultiStimuli::sendStimulationInfo()
{
    int stim = 0;
    int setCount = 0;

    if(m_ERP->stimulationType() == speller_type::DUAL_STIM)
    {
        setCount = 2;
    }
    else if(m_ERP->stimulationType() == speller_type::MULTI_STIM)
    {
        setCount = 3;
    }
    sendMarker(OVTK_StimulationId_VisualStimulationStart);

    // for(int i=0; i<3; i++)
    for(int i=0; i<setCount; i++)
    {
        stim = m_flashingSequence->sequence[m_currentStimulation+i];
        if  (stim == 0)
        {
            break;
        }
        // sendMarker(config::OVTK_StimulationLabel_Base + m_flashingSequence->sequence[m_currentStimulation+i]);
        sendMarker(config::OVTK_StimulationLabel_Base + stim);
        // send target marker
        if (m_ERP->experimentMode() == operation_mode::CALIBRATION ||
                m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {
            if (isTarget(stim))
            {
                sendMarker(OVTK_StimulationId_Target);
            }
            else
            {
                sendMarker(OVTK_StimulationId_NonTarget);
            }
        }
    }
}

void MultiStimuli::updateWidgets(QList<QPixmap> &pics)
{
    // qDebug()<< Q_FUNC_INFO;
    int id = 0;
    int setCount = 0;

    if(m_ERP->stimulationType() == speller_type::DUAL_STIM)
    {
        setCount = 2;
    }
    else if(m_ERP->stimulationType() == speller_type::MULTI_STIM)
    {
        setCount = 3;
    }

    for(int i=0; i<setCount; i++)

    {
        id = m_flashingSequence->sequence[m_currentStimulation+i]-1;
        if (id == -1)
        {
            break;
        }
        // qDebug()<< Q_FUNC_INFO << id;
        // QPixmap pixmap = pics[id-1];
        QPixmap pixmap = pics[id];
        m_element = new QLabel();
        m_element->setPixmap(pixmap);
        m_element->setAlignment(Qt::AlignCenter);

        this->layout()->replaceWidget(this->
                                      layout()->
                                      itemAt(id)->
                                      widget(),
                                      m_element,
                                      Qt::FindDirectChildrenOnly);
    }
}

MultiStimuli::~MultiStimuli()
{

}

