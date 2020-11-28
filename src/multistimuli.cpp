#include <QLayout>
#include <QTimer>
#include <QTime>
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
    int currentStim = m_flashingSequence->sequence[m_currentStimulation];
    //qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    if(currentStim <= 3)
    {
       this->layout()->itemAt(1)->
                widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_red_inverted.png)");

        this->layout()->itemAt(2)->
                 widget()->setStyleSheet("qproperty-pixmap: url(:/images/yin_yang_small_croped.png)");

        this->layout()->itemAt(3)->
                 widget()->setStyleSheet("qproperty-pixmap: url(:/images/whitehouse_small.png)");
    }

    else if (currentStim <= 6)
    {

        this->layout()->itemAt(4)->
                 widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_blue_inverted.png)");

         this->layout()->itemAt(5)->
                  widget()->setStyleSheet("qproperty-pixmap: url(:/images/yin_yang_small_croped.png)");

         this->layout()->itemAt(6)->
                  widget()->setStyleSheet("qproperty-pixmap: url(:/images/whitehouse_small.png)");

    }
    else if (currentStim <= 9)
    {

        this->layout()->itemAt(7)->
                 widget()->setStyleSheet("qproperty-pixmap: url(:/images/bennabi_face_magenta_inverted.png)");

         this->layout()->itemAt(8)->
                  widget()->setStyleSheet("qproperty-pixmap: url(:/images/yin_yang_small_croped.png)");

         this->layout()->itemAt(9)->
                  widget()->setStyleSheet("qproperty-pixmap: url(:/images/whitehouse_small.png)");
    }

    switchStimulationTimers();
}
void MultiStimuli::pauseFlashing()
{
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

   int currentStim = m_flashingSequence->sequence[m_currentStimulation];

   if(currentStim <= 3)
   {

       for (int i=1;i<=3;i++) {
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

   else if (currentStim <= 6)
   {

       for (int i=4;i<=6;i++) {
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
   else if (currentStim <= 9)
   {

       for (int i=7;i<=9;i++) {
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


    switchStimulationTimers();
    ++m_currentStimulation;

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
