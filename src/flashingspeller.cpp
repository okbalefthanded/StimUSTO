#include <QLayout>
#include <QTimer>
//
#include "flashingspeller.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//
void FlashingSpeller::startFlashing()
{

    sendStimulationInfo();

    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("QLabel { color : white; font: 60pt }");

    switchStimulationTimers();
}

