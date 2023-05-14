//
#include <QtDebug>
//
#include "doubleerp.h"
#include "utils.h"
//

DoubleERP::DoubleERP()
{
    m_type = paradigm_type::DOUBLE_ERP;

    m_1stParadigm = new ERP();
    m_2ndParadigm = new ERP();
}

DoubleERP::DoubleERP(ERP *first, ERP *second)
{
    m_type = paradigm_type::HYBRID;

    m_1stParadigm = new ERP(first->experimentMode(),
                            first->controlMode(),
                            first->type(),
                            external_comm::DISABLED,
                            first->stimulationDuration(),
                            first->breakDuration(),
                            first->nrSequences(),
                            first->desiredPhrase(),
                            m_externalAddress,
                            first->stimulationType(),
                            first->flashingMode()
                            );

    m_2ndParadigm = new ERP(second->experimentMode(),
                            second->controlMode(),
                            second->type(),
                            external_comm::DISABLED,
                            second->stimulationDuration(),
                            second->breakDuration(),
                            second->nrSequences(),
                            second->desiredPhrase(),
                            m_externalAddress,
                            second->stimulationType(),
                            second->flashingMode()
                            );
}

QVariant DoubleERP::toVariant() const
{
    QVariantMap map;

    map.insert("paradigmType", m_type);
    map.insert("experimentMode", m_experimentMode);
    map.insert("externalComm", m_externalComm);
    map.insert("ip", m_externalAddress);
    // 1st ERP config
    map.insert("1st_breakDuration",  m_1stParadigm->breakDuration());
    map.insert("1st_desiredPhrase",  m_1stParadigm->desiredPhrase());
    map.insert("1st_experimentMode", m_experimentMode);
    // map.insert("ERP_controlMode", m_ERPparadigm->controlMode());
    map.insert("1st_flashingMode",   m_1stParadigm->flashingMode());
    map.insert("1st_nrSequences",    m_1stParadigm->nrSequences());
    map.insert("1st_paradigmType",   m_1stParadigm->type());
    map.insert("1st_stimulationDuration", m_1stParadigm->stimulationDuration());
    map.insert("1st_stimulationType", m_1stParadigm->stimulationType());

    // 2nd ERP config
    map.insert("2nd_breakDuration",  m_2ndParadigm->breakDuration());
    map.insert("2nd_desiredPhrase",  m_2ndParadigm->desiredPhrase());
    map.insert("2nd_experimentMode", m_experimentMode);
    // map.insert("ERP_controlMode", m_ERPparadigm->controlMode());
    map.insert("2nd_flashingMode",   m_2ndParadigm->flashingMode());
    map.insert("2nd_nrSequences",    m_2ndParadigm->nrSequences());
    map.insert("2nd_paradigmType",   m_2ndParadigm->type());
    map.insert("2nd_stimulationDuration", m_2ndParadigm->stimulationDuration());
    map.insert("2nd_stimulationType", m_2ndParadigm->stimulationType());

    return map;
}

void DoubleERP::fromVariant(const QVariant &variant)
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

    // 1st ERP config
    m_1stParadigm->setExperimentMode(map.value("experimentMode").toInt());
    // m_1stParadigm->setControlMode(map.value("1st_controlMode").toInt());
    m_1stParadigm->setType(map.value("1st_paradigmType").toInt());
    m_1stParadigm->setStimulationDuration(map.value("1st_stimulationDuration").toInt());
    m_1stParadigm->setBreakDuration(map.value("1st_breakDuration").toInt());
    m_1stParadigm->setNrSequences(map.value("1st_nrSequences").toInt());
    m_1stParadigm->setStimulationType(map.value("1st_stimulationType").toInt());
    m_1stParadigm->setFlashingMode(map.value("1st_flashingMode").toInt());
    QString str = map.value("1st_desiredPhrase").toString();
    QString randomStr;

    if(str.isEmpty())
    {
       randomStr = getRandomSequence(m_1stParadigm, 4);
       m_1stParadigm->setDesiredPhrase(randomStr);
    }
    else
    {
        m_1stParadigm->setDesiredPhrase(str);
    }

    // 2nd ERP config
    m_2ndParadigm->setExperimentMode(map.value("experimentMode").toInt());
    m_2ndParadigm->setExperimentMode(map.value("experimentMode").toInt());
    // m_2ndParadigm->setControlMode(map.value("2nd_controlMode").toInt());
    m_2ndParadigm->setType(map.value("2nd_paradigmType").toInt());
    m_2ndParadigm->setStimulationDuration(map.value("2nd_stimulationDuration").toInt());
    m_2ndParadigm->setBreakDuration(map.value("2nd_breakDuration").toInt());
    m_2ndParadigm->setNrSequences(map.value("2nd_nrSequences").toInt());
    m_2ndParadigm->setStimulationType(map.value("2nd_stimulationType").toInt());
    m_2ndParadigm->setFlashingMode(map.value("2nd_flashingMode").toInt());
    QString randomStr2;
    str = map.value("2nd_desiredPhrase").toString();
    if(str.isEmpty())
    {
        randomStr2 = getRandomSequence(m_2ndParadigm, 6);
        m_2ndParadigm->setDesiredPhrase(randomStr2);
    }
    else
    {
        m_2ndParadigm->setDesiredPhrase(map.value("2nd_desiredPhrase").toString());
    }

}

QString DoubleERP::getRandomSequence(ERP *paradigm, int repetitions)
{
    int elements = 9;

    if (paradigm->stimulationType() == speller_type::AUDITORY)
    {
        elements = 5;
    }

    else if (paradigm->stimulationType() >= speller_type::SMALL && paradigm->stimulationType() <= speller_type::SMALL_CIRCLE)
    {
        elements = 6;
    }

    RandomFlashSequence *randomPhrase = new RandomFlashSequence(elements, repetitions);
    return randomPhrase->toString();
}

DoubleERP::~DoubleERP(){}
