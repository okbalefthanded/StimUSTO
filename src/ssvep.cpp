//
#include <QDebug>
//
#include "ssvep.h"
#include "utils.h"
//
SSVEP::SSVEP() : Paradigm (),
    m_nrElements(5), m_frequencies("6.66,7.5,8.57,10")
{
}

SSVEP::SSVEP(quint8 mode, quint8 type, int dur, quint8 bDur,
             quint8 nrSeq, QString phrase, quint8 nElements,
             QString frequnecies, quint8 stimulationMode):
    Paradigm(mode, type, dur, bDur, nrSeq, phrase),
    m_nrElements(nElements), m_frequencies(frequnecies), m_stimulationMode(stimulationMode)
{
}

QVariant SSVEP::toVariant() const
{
    QVariantMap map;

    map.insert("experimentMode", m_experimentMode);
    map.insert("paradigmType", m_type);
    map.insert("stimulationDuration", m_stimulationDuration);
    map.insert("breakDuration", m_breakDuration);
    map.insert("nrSequences", m_nrSequences);
    map.insert("desiredPhrase", m_desiredPhrase);
    map.insert("nrElements", m_nrElements);
    map.insert("frequencies", m_frequencies);

    return map;
}

void SSVEP::fromVariant(const QVariant &variant)
{

    QVariantMap map = variant.toMap();
    m_experimentMode = map.value("experimentMode").toInt();
    m_type = map.value("paradigmType").toInt();
    m_stimulationDuration = map.value("stimulationDuration").toInt();
    m_breakDuration = map.value("breakDuration").toInt();
    m_nrSequences = map.value("nrSequences").toInt();
    m_desiredPhrase = map.value("desiredPhrase").toString();
    m_nrElements = map.value("nrElements").toInt();
    m_frequencies = map.value("frequencies").toString();
    m_stimulationMode = map.value("stimulationMode").toInt();
}

quint8 SSVEP::nrElements() const
{
    return m_nrElements;
}

void SSVEP::setNrElements(const quint8 &nrElements)
{
    m_nrElements = nrElements;
}

QString SSVEP::frequencies() const
{
    return m_frequencies;
}

void SSVEP::setFrequencies(const QString &frequencies)
{
    m_frequencies = frequencies;
}

quint8 SSVEP::stimulationMode() const
{
    return m_stimulationMode;
}

void SSVEP::setStimulationMode(const quint8 &stimulationMode)
{
    m_stimulationMode = stimulationMode;
}
