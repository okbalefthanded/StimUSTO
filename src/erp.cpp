#include "erp.h"
#include "utils.h"

ERP::ERP() : Paradigm (),
             m_stimulationType(speller_type::INVERTED_FACE), m_flashingMode(flashing_mode::SC)
{

}

ERP::ERP(quint8 mode, quint8 control, quint8 type, int dur, quint8 bDur,
         quint8 nrSeq, QString phrase, quint8 sType, quint8 fMode):
        Paradigm(mode, control, type, dur, bDur, nrSeq, phrase),
        m_stimulationType(sType), m_flashingMode(fMode)
{

}

QVariant ERP::toVariant() const
{
    QVariantMap map;

    map.insert("experimentMode", m_experimentMode);
    map.insert("controlMode", m_controlMode);
    map.insert("paradigmType", m_type);
    map.insert("stimulationDuration", m_stimulationDuration);
    map.insert("breakDuration", m_breakDuration);
    map.insert("nrSequences", m_nrSequences);
    map.insert("desiredPhrase", m_desiredPhrase);
    map.insert("stimulationType", m_stimulationType);
    map.insert("flashingMode", m_flashingMode);

    return map;

}

void ERP::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    m_experimentMode = map.value("experimentMode").toInt();
    m_controlMode = map.value("controlMode").toInt();
    m_type = map.value("paradigmType").toInt();
    m_stimulationDuration = map.value("stimulationDuration").toInt();
    m_breakDuration = map.value("breakDuration").toInt();
    m_nrSequences = map.value("nrSequences").toInt();
    m_desiredPhrase = map.value("desiredPhrase").toString();
    m_stimulationType = map.value("stimulationType").toInt();
    m_flashingMode = map.value("flashingMode").toInt();
}


quint8 ERP::stimulationType() const
{
    return m_stimulationType;
}

void ERP::setStimulationType(const quint8 &t_stimulationType)
{
    m_stimulationType = t_stimulationType;
}

quint8 ERP::flashingMode() const
{
    return m_flashingMode;
}

void ERP::setFlashingMode(const quint8 &t_flashingMode)
{
    m_flashingMode = t_flashingMode;
}
