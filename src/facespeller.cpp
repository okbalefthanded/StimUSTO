#include <QLayout>
#include <QTimer>
//
#include "facespeller.h"
#include "erp.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//
void FaceSpeller::startFlashing()
{

    sendStimulationInfo();

    switch(m_spellerType)
    {
    case speller_type::FACES_SPELLER:
    {
        stimulationFace();
        break;
    }
    case speller_type::INVERTED_FACE:
    {
        stimulationInvertedFace();
        break;
    }
    case speller_type::COLORED_FACE :
    {
        stimulationColoredFace();
        break;
    }
    case speller_type::INVERTED_COLORED_FACE:
    {
        stimulationInvertedColoredFace();
        break;
    }
    }

    switchStimulationTimers();
}

void FaceSpeller::initSpeller(ERP *prdg)
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

void FaceSpeller::stimulationFace()
{
    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("image: url(:/images/bennabi_face.png)");
}

void FaceSpeller::stimulationColoredFace()
{
    switch (m_flashingSequence->sequence[m_currentStimulation])
    {
    case 1:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_blue.png)");
        break;
    }
    case 2:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_green.png)");
        break;
    }
    case 3:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_red.png)");
        break;
    }
    case 4:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_brown.png)");
        break;
    }
    case 5:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_cyan.png)");
        break;
    }
    case 6:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face.png)");
        break;
    }
    case 7:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_yellow.png)");
        break;
    }
    case 8:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_orange.png)");
        break;
    }
    case 9:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_magenta.png)");
        break;
    }

    }

}

void FaceSpeller::stimulationInvertedFace()
{
    this->layout()
            ->itemAt(m_flashingSequence->sequence[m_currentStimulation])
            ->widget()
            ->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");
}

void FaceSpeller::stimulationInvertedColoredFace()
{
    switch (m_flashingSequence->sequence[m_currentStimulation])
    {
    case 1:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_blue_inverted.png)");
        break;
    }
    case 2:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_green_inverted.png)");
        break;
    }
    case 3:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_red_inverted.png)");
        break;
    }
    case 4:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_brown_inverted.png)");
        break;
    }
    case 5:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_cyan_inverted.png)");
        break;
    }
    case 6:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");
        break;
    }
    case 7:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_yellow_inverted.png)");
        break;
    }
    case 8:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_orange_inverted.png)");
        break;
    }
    case 9:
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_magenta_inverted.png)");
        break;
    }

    }
}
