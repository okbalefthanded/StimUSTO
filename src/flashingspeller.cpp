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

void FlashingSpeller::initSpeller(ERP *prdg)
{
    m_stimulationDuration = prdg->stimulationDuration();
    m_isi = prdg->breakDuration();
    m_nrSequence = prdg->nrSequences();
    m_spellingMode = prdg->experimentMode();
    m_desiredPhrase = prdg->desiredPhrase();
    m_spellerType = prdg->stimulationType();
    m_feedbackPort = 12345;

    m_stimTimer->setInterval(m_stimulationDuration);
    m_isiTimer->setInterval(m_isi);
}
