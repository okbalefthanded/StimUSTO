//
#include <QLayout>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QWindow>
#include <QtWidgets>
//
#include "arabicspeller.h"
#include "erp.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//


void ArabicSpeller::pauseFlashing()
{
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

     this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                  widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

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


void ArabicSpeller::startFlashing()
{
    qDebug()<< Q_FUNC_INFO;
    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");

    switchStimulationTimers();
}
