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
void MultiStimuli::startFlashing()
{
    sendStimulationInfo();
    // stimulation
    updateWidgets(m_stimuli);

    switchStimulationTimers();
}

void MultiStimuli::pauseFlashing()
{
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    updateWidgets(m_icons);

    switchStimulationTimers();
    // ++m_currentStimulation;
    m_currentStimulation = m_currentStimulation + (m_nrElements / 3);
    trialEnd();

}

void MultiStimuli::preTrial()
{
    qDebug()<< Q_FUNC_INFO;

    if(m_trials == 0)
    {
        sendMarker(OVTK_StimulationId_ExperimentStart);

        foreach(QString value, m_stimuliMap)
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
        m_flashingSequence = new RandomFlashSequence(m_nrElements, m_ERP->nrSequences(), 3, 3);

        // qDebug() << Q_FUNC_INFO << m_flashingSequence->sequence;

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

void MultiStimuli::createLayout()
{
    // create special laytout

}

void MultiStimuli::sendStimulationInfo()
{
    int stim = 0;
    sendMarker(OVTK_StimulationId_VisualStimulationStart);

    for(int i=0; i<3; i++)
    {
        stim = m_flashingSequence->sequence[m_currentStimulation+i];
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
    int id = 0;

    for(int i=0; i<3; i++)
    {
        id = m_flashingSequence->sequence[m_currentStimulation+i];
        QPixmap pixmap = pics[id-1];
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

