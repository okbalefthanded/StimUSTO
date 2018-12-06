#include "speller.cpp"
#include "test.h"
#include "ovtk_stimulations.h"
#include "utils.h"

Test::Test()
{
    Speller();
}

void Test::startFlashing()
{
    sendMarker(OVTK_StimulationId_VisualStimulationStart);
    sendMarker(config::OVTK_StimulationLabel_Base + m_flashingSequence->sequence[m_currentStimulation]);

    // send target marker
    if (m_spellingMode == operation_mode::CALIBRATION ||
            m_spellingMode == operation_mode::COPY_MODE)
    {
        if (isTarget())
        {
            // qDebug()<< desired_phrase[currentLetter];
            sendMarker(OVTK_StimulationId_Target);
        }
        else
        {
            sendMarker(OVTK_StimulationId_NonTarget);
        }
    }

    if(m_spellerType == speller_type::FACES_SPELLER)
    {

        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face.png)");

    }
    else if(m_spellerType == speller_type::INVERTED_FACE)
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");
    }
    else if(m_spellerType == speller_type::FLASHING_SPELLER)
    {
        this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation])->
                widget()->setStyleSheet("QLabel { color : white; font: 60pt }");
    }

    m_stimTimer->start();
    m_isiTimer->stop();
    m_state = trial_state::POST_STIMULUS;
}
