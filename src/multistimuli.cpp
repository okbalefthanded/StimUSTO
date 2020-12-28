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
    int currentStim1 = m_flashingSequence->sequence[m_currentStimulation];
    int currentStim2 = m_flashingSequence->sequence[m_currentStimulation+1];
    int currentStim3 = m_flashingSequence->sequence[m_currentStimulation+2];

    qDebug()<< Q_FUNC_INFO << currentStim1 << currentStim2 << currentStim3;

    this->layout()->itemAt(currentStim1)->
            widget()->setStyleSheet("qproperty-pixmap: url(:/images/"+stimuli[currentStim1]+")");

    this->layout()->itemAt(currentStim2)->
            widget()->setStyleSheet("qproperty-pixmap: url(:/images/"+stimuli[currentStim2]+")");

    this->layout()->itemAt(currentStim3)->
            widget()->setStyleSheet("qproperty-pixmap: url(:/images/"+stimuli[currentStim3]+")");

    /*
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    //if(currentStim <= 3)
    if(currentStim == 1)
    {
        this->layout()->itemAt(1)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_red_inverted.png)");

        this->layout()->itemAt(2)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/yin_yang_small_croped.png)");

        this->layout()->itemAt(3)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/whitehouse_small.png)");
    }

    //else if (currentStim <= 6)
    else if (currentStim == 2)
    {

        this->layout()->itemAt(4)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_blue_inverted.png)");

        this->layout()->itemAt(5)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/yin_yang_small_croped.png)");

        this->layout()->itemAt(6)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/whitehouse_small.png)");

    }
    // else if (currentStim <= 9)
    else if (currentStim == 3)
    {

        this->layout()->itemAt(7)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_magenta_inverted.png)");

        this->layout()->itemAt(8)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/yin_yang_small_croped.png)");

        this->layout()->itemAt(9)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/whitehouse_small.png)");
    }
    */
    switchStimulationTimers();
}

void MultiStimuli::pauseFlashing()
{
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    int currentStim1 = m_flashingSequence->sequence[m_currentStimulation];
    int currentStim2 = m_flashingSequence->sequence[m_currentStimulation+1];
    int currentStim3 = m_flashingSequence->sequence[m_currentStimulation+2];

    qDebug()<< Q_FUNC_INFO << currentStim1 << currentStim2 << currentStim3;

    m_element = new QLabel();
    m_element->setPixmap(m_icons[currentStim1-1]);
    m_element->setAlignment(Qt::AlignCenter);
    this->layout()->replaceWidget(this->
                                  layout()->
                                  itemAt(currentStim1)->
                                  widget(),
                                  m_element,
                                  Qt::FindDirectChildrenOnly);
    m_element = new QLabel();
    m_element->setPixmap(m_icons[currentStim2-1]);
    m_element->setAlignment(Qt::AlignCenter);
    this->layout()->replaceWidget(this->
                                  layout()->
                                  itemAt(currentStim2)->
                                  widget(),
                                  m_element,
                                  Qt::FindDirectChildrenOnly);

    m_element = new QLabel();
    m_element->setPixmap(m_icons[currentStim3-1]);
    m_element->setAlignment(Qt::AlignCenter);
    this->layout()->replaceWidget(this->
                                  layout()->
                                  itemAt(currentStim3)->
                                  widget(),
                                  m_element,
                                  Qt::FindDirectChildrenOnly);


    /*
   // if(currentStim <= 3)
   if(currentStim == 1)
   {

       for (int i=1; i<=3; i++) {
           m_element = new QLabel();
           m_element->setPixmap(m_icons[i-1]);
           m_element->setAlignment(Qt::AlignCenter);

           this->layout()->replaceWidget(this->
                                         layout()->
                                         itemAt(i)->
                                         widget(),
                                         m_element,
                                         Qt::FindDirectChildrenOnly);
       }


   }

   // else if (currentStim <= 6)
   else if (currentStim == 2)
   {

       for (int i=4; i<=6; i++) {
           m_element = new QLabel();
           m_element->setPixmap(m_icons[i-1]);
           m_element->setAlignment(Qt::AlignCenter);

           this->layout()->replaceWidget(this->
                                         layout()->
                                         itemAt(i)->
                                         widget(),
                                         m_element,
                                         Qt::FindDirectChildrenOnly);
       }


   }
   // else if (currentStim <= 9)
   else if (currentStim == 3)
   {

       for (int i=7; i<=9; i++) {
           m_element = new QLabel();
           m_element->setPixmap(m_icons[i-1]);
           m_element->setAlignment(Qt::AlignCenter);

           this->layout()->replaceWidget(this->
                                         layout()->
                                         itemAt(i)->
                                         widget(),
                                         m_element,
                                         Qt::FindDirectChildrenOnly);
       }

   }
   */
    switchStimulationTimers();
    // ++m_currentStimulation;
    m_currentStimulation = m_currentStimulation + (m_nrElements / 3);
    qDebug()<< m_currentStimulation;

    if (m_currentStimulation >= m_flashingSequence->sequence.count())
    {
        ++m_currentLetter;
        m_isiTimer->stop();
        m_stimTimer->stop();

        // utils::wait(1000); // time window after last epoch/stim
        // utils::wait(500);
        utils::wait(700); // 700 ms == epoch time windows
        sendMarker(OVTK_StimulationId_TrialStop);
        m_state = trial_state::FEEDBACK;

        if(m_ERP->experimentMode() == operation_mode::COPY_MODE || m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            feedback();
        }
        else if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            postTrial();
        }
    }
}


void MultiStimuli::preTrial()
{
    qDebug()<< Q_FUNC_INFO;

    if(m_trials == 0)
    {
        sendMarker(OVTK_StimulationId_ExperimentStart);
        //        initLogger();
    }

    if (m_preTrialCount == 0)
    {
        // Refresh previous feedback
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
                /*
                this->layout()->replaceWidget(this->
                                              layout()->
                                              itemAt(id)->
                                              widget(),
                                              m_element,
                                              Qt::FindDirectChildrenOnly);*/
            }
        }
        //
        sendMarker(OVTK_StimulationId_TrialStart);
        //m_flashingSequence = new RandomFlashSequence(m_nrElements / 3, m_ERP->nrSequences());
        m_flashingSequence = new RandomFlashSequence(m_nrElements, m_ERP->nrSequences(), 3, 3);

        qDebug() << Q_FUNC_INFO << m_flashingSequence->sequence;

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
