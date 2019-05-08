//
#include <QtDebug>
//
#include "hybrid.h"
//

Hybrid::Hybrid()
{
    m_ERPparadigm = new ERP();
    m_SSVEPparadigm = new SSVEP();
}

QVariant Hybrid::toVariant() const
{
    QVariantMap map;

    // ERP config
    map.insert("ERP_breakDuration", m_ERPparadigm->breakDuration());
    map.insert("ERP_desiredPhrase", m_ERPparadigm->desiredPhrase());
    map.insert("ERP_experimentMode",m_ERPparadigm->experimentMode());
    // map.insert("SSVEP_controlMode", m_ERPparadigm->controlMode());
    map.insert("ERP_flashingMode", m_ERPparadigm->flashingMode());
    map.insert("ERP_nrSequences", m_ERPparadigm->nrSequences());
    map.insert("ERP_paradigmType", m_ERPparadigm->type());
    map.insert("ERP_stimulationDuration", m_ERPparadigm->stimulationDuration());
    map.insert("ERP_stimulationType", m_ERPparadigm->stimulationType());
    // SSVEP config
    map.insert("SSVEP_breakDuration", m_SSVEPparadigm->breakDuration());
    map.insert("SSVEP_desiredPhrase", m_SSVEPparadigm->desiredPhrase());
    map.insert("SSVEP_experimentMode", m_SSVEPparadigm->experimentMode());
    map.insert("SSVEP_controlMode", m_SSVEPparadigm->controlMode());
    map.insert("SSVEP_frequencies", m_SSVEPparadigm->frequencies());
    map.insert("SSVEP_nrElements", m_SSVEPparadigm->nrElements());
    map.insert("SSVEP_nrSequences", m_SSVEPparadigm->nrSequences());
    map.insert("SSVEP_paradigmType", m_SSVEPparadigm->type());
    map.insert("SSVEP_stimulationDuration", m_SSVEPparadigm->stimulationDuration());
    map.insert("SSVEP_stimulationMode", m_SSVEPparadigm->stimulationMode());

    return map;
}

void Hybrid::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    // ERP config
    m_ERPparadigm->setExperimentMode(map.value("ERP_experimentMode").toInt());
    m_ERPparadigm->setControlMode(map.value("ERP_controlMode").toInt());
    m_ERPparadigm->setType(map.value("ERP_paradigmType").toInt());
    m_ERPparadigm->setStimulationDuration(map.value("ERP_stimulationDuration").toInt());
    m_ERPparadigm->setBreakDuration(map.value("ERP_breakDuration").toInt());
    m_ERPparadigm->setNrSequences(map.value("ERP_nrSequences").toInt());
    m_ERPparadigm->setDesiredPhrase(map.value("ERP_desiredPhrase").toString());
    m_ERPparadigm->setStimulationType(map.value("ERP_stimulationType").toInt());
    m_ERPparadigm->setFlashingMode(map.value("ERP_flashingMode").toInt());
    // SSVEP config
    m_SSVEPparadigm->setExperimentMode(map.value("SSVEP_experimentMode").toInt());
    m_SSVEPparadigm->setControlMode(map.value("SSVEP_controlMode").toInt());
    m_SSVEPparadigm->setType(map.value("paradigmType").toInt());
    m_SSVEPparadigm->setStimulationDuration(map.value("SSVEP_stimulationDuration").toInt());
    m_SSVEPparadigm->setBreakDuration(map.value("SSVEP_breakDuration").toInt());
    m_SSVEPparadigm->setNrSequences(map.value("SSVEP_nrSequences").toInt());
    m_SSVEPparadigm->setDesiredPhrase(map.value("SSVEP_desiredPhrase").toString());
    m_SSVEPparadigm->setNrElements(map.value("SSVEP_nrElements").toInt());
    m_SSVEPparadigm->setFrequencies(map.value("SSVEP_frequencies").toString());
    m_SSVEPparadigm->setStimulationMode(map.value("SSVEP_stimulationMode").toInt());
}
