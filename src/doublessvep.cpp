//
#include <QtDebug>
//
#include "doublessvep.h"
#include "utils.h"
//

DoubleSSVEP::DoubleSSVEP()
{
    m_type = paradigm_type::DOUBLE_SSVEP;

    m_1stParadigm = new SSVEP();
    m_2ndParadigm = new SSVEP();
}

DoubleSSVEP::DoubleSSVEP(SSVEP *first, SSVEP *second)
{
    m_type = paradigm_type::DOUBLE_SSVEP;

    m_1stParadigm = new SSVEP(first->experimentMode(),
                              first->controlMode(),
                              first->type(),
                              external_comm::DISABLED,
                              first->stimulationDuration(),
                              first->breakDuration(),
                              first->nrSequences(),
                              first->stimulationType(),
                              first->desiredPhrase(),
                              m_externalAddress,
                              first->nrElements(),
                              first->frequencies(),
                              first->stimulationMode()
                              );

    m_2ndParadigm = new SSVEP(second->experimentMode(),
                              second->controlMode(),
                              second->type(),
                              external_comm::DISABLED,
                              second->stimulationDuration(),
                              second->breakDuration(),
                              second->nrSequences(),
                              second->stimulationType(),
                              second->desiredPhrase(),
                              m_externalAddress,
                              second->nrElements(),
                              second->frequencies(),
                              second->stimulationMode()
                              );
}


QVariant DoubleSSVEP::toVariant() const
{
    QVariantMap map;

    map.insert("paradigmType", m_type);
    map.insert("experimentMode", m_experimentMode);
    map.insert("externalComm", m_externalComm);
    map.insert("ip", m_externalAddress);
    // 1st SSSVEP config
    map.insert("1st_breakDuration", m_1stParadigm->breakDuration());
    map.insert("1st_desiredPhrase", m_1stParadigm->desiredPhrase());
    map.insert("1st_experimentMode", m_experimentMode);
    map.insert("1st_controlMode", m_1stParadigm->controlMode());
    map.insert("1st_frequencies", m_1stParadigm->frequencies());
    map.insert("1st_nrElements", m_1stParadigm->nrElements());
    map.insert("1st_nrSequences", m_1stParadigm->nrSequences());
    map.insert("1st_paradigmType", m_1stParadigm->type());
    map.insert("1st_stimulationType", m_1stParadigm->stimulationType());
    map.insert("1st_stimulationDuration", m_1stParadigm->stimulationDuration());
    map.insert("1st_stimulationMode", m_1stParadigm->stimulationMode());
    // 2nd SSVEP config
    map.insert("2nd_breakDuration", m_2ndParadigm->breakDuration());
    map.insert("2nd_desiredPhrase", m_2ndParadigm->desiredPhrase());
    map.insert("2nd_experimentMode", m_experimentMode);
    map.insert("2nd_controlMode", m_2ndParadigm->controlMode());
    map.insert("2nd_frequencies", m_2ndParadigm->frequencies());
    map.insert("2nd_nrElements", m_2ndParadigm->nrElements());
    map.insert("2nd_nrSequences", m_2ndParadigm->nrSequences());
    map.insert("2nd_paradigmType", m_2ndParadigm->type());
    map.insert("2nd_stimulationType", m_2ndParadigm->stimulationType());
    map.insert("2nd_stimulationDuration", m_2ndParadigm->stimulationDuration());
    map.insert("2nd_stimulationMode", m_2ndParadigm->stimulationMode());

    return map;
}

void DoubleSSVEP::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    m_type = map.value("paradigymType").toUInt();
    m_experimentMode = map.value("experimentMode").toUInt();

    if(map.value("externalComm").isNull())
    {
        m_externalComm    = external_comm::DISABLED;
        m_externalAddress = "127.0.0.1"; // home sweet home
    }
    else
    {
        m_externalComm    = map.value("externalComm").toInt();
        m_externalAddress = map.value("ip").toString();
    }

    // 1st SSVEP config
    m_1stParadigm->setExperimentMode(map.value("experimentMode").toInt());
    m_1stParadigm->setControlMode(map.value("1st_controlMode").toInt());
    m_1stParadigm->setType(map.value("paradigmType").toInt());
    m_1stParadigm->setStimulationDuration(map.value("1st_stimulationDuration").toInt());
    m_1stParadigm->setBreakDuration(map.value("1st_breakDuration").toInt());
    m_1stParadigm->setNrSequences(map.value("1st_nrSequences").toInt());
    m_1stParadigm->setNrElements(map.value("1st_nrElements").toInt());
    m_1stParadigm->setFrequencies(map.value("1st_frequencies").toString());
    m_1stParadigm->setStimulationMode(map.value("1st_stimulationMode").toInt());
    m_1stParadigm->setStimulationType(map.value("1st_stimulationType").toInt());
    QString str = map.value("1st_desiredPhrase").toString();

    if(str.isEmpty())
    {
        //   RandomFlashSequence *rfseq = new RandomFlashSequence(m_SSVEPparadigm->nrElements(),
        //                                                       m_SSVEPparadigm->nrSequences() / m_SSVEPparadigm->nrElements());
        int n_ssvepSequences = m_1stParadigm->nrSequences() / m_1stParadigm->nrElements();
        RandomFlashSequence *rfseq = new RandomFlashSequence(m_1stParadigm->nrElements(),
                                                             n_ssvepSequences); // 6
        m_1stParadigm->setDesiredPhrase(rfseq->toString());
    }
    else
    {
        m_1stParadigm->setDesiredPhrase(map.value("1st_desiredPhrase").toString());
    }

    // 2nd SSVEP config
    if (map.contains("2nd_experimentMode")) // for ASYNC
    {
        m_2ndParadigm->setExperimentMode(map.value("2nd_experimentMode").toInt());
    }
    else
    {
        m_2ndParadigm->setExperimentMode(map.value("experimentMode").toInt());
    }
    m_2ndParadigm->setControlMode(map.value("2nd_controlMode").toInt());
    m_2ndParadigm->setType(map.value("paradigmType").toInt());
    m_2ndParadigm->setStimulationDuration(map.value("2nd_stimulationDuration").toInt());
    m_2ndParadigm->setBreakDuration(map.value("2nd_breakDuration").toInt());
    m_2ndParadigm->setNrSequences(map.value("2nd_nrSequences").toInt());
    m_2ndParadigm->setNrElements(map.value("2nd_nrElements").toInt());
    m_2ndParadigm->setFrequencies(map.value("2nd_frequencies").toString());
    m_2ndParadigm->setStimulationMode(map.value("2nd_stimulationMode").toInt());
    m_2ndParadigm->setStimulationType(map.value("2nd_stimulationType").toInt());

    str = map.value("2nd_desiredPhrase").toString();
    if(str.isEmpty())
    {
        //   RandomFlashSequence *rfseq = new RandomFlashSequence(m_SSVEPparadigm->nrElements(),
        //                                                       m_SSVEPparadigm->nrSequences() / m_SSVEPparadigm->nrElements());
        int n_ssvepSequences = m_2ndParadigm->nrSequences() / m_2ndParadigm->nrElements();
        RandomFlashSequence *rfseq = new RandomFlashSequence(m_2ndParadigm->nrElements(),
                                                             n_ssvepSequences); // 6
        m_2ndParadigm->setDesiredPhrase(rfseq->toString());
    }
    else
    {
        m_2ndParadigm->setDesiredPhrase(map.value("2nd_desiredPhrase").toString());
    }
}

DoubleSSVEP::~DoubleSSVEP(){}
