#include <QVariantMap>
//
#include "paradigm.h"
#include "utils.h"
//
Paradigm::Paradigm(quint8 mode, quint8 type, int dur, quint8 bDur, quint8 nrSeq, QString phrase):
    m_experimentMode(mode), m_type(type), m_stimulationDuration(dur), m_breakDuration(bDur), m_nrSequences(nrSeq),
    m_desiredPhrase(phrase)
{

}

Paradigm::Paradigm():
    m_experimentMode(operation_mode::CALIBRATION), m_type(paradigm_type::ERP), m_stimulationDuration(100),
    m_breakDuration(100), m_nrSequences(10), m_desiredPhrase("12345")
{

}

Paradigm::~Paradigm()
{

}


int Paradigm::experimentMode() const
{
    return m_experimentMode;
}

void Paradigm::setExperimentMode(int t_experimentMode)
{
    m_experimentMode = t_experimentMode;
}

int Paradigm::type() const
{
    return m_type;
}

void Paradigm::setType(int t_type)
{
    m_type = t_type;
}

int Paradigm::stimulationDuration() const
{
    return m_stimulationDuration;
}

void Paradigm::setStimulationDuration(int t_stimulationDuration)
{
    m_stimulationDuration = t_stimulationDuration;
}

int Paradigm::breakDuration() const
{
    return m_breakDuration;
}

void Paradigm::setBreakDuration(int t_breakDuration)
{
    m_breakDuration = t_breakDuration;
}

int Paradigm::nrSequences() const
{
    return m_nrSequences;
}

void Paradigm::setNrSequences(int t_nrSequences)
{
    m_nrSequences = t_nrSequences;
}

QString Paradigm::desiredPhrase() const
{
    return m_desiredPhrase;
}

void Paradigm::setDesiredPhrase(const QString &t_desiredPhrase)
{
    m_desiredPhrase = t_desiredPhrase;
}
