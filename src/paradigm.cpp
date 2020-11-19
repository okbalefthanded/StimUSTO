#include <QVariantMap>
//
#include "paradigm.h"
#include "utils.h"
//
Paradigm::Paradigm(quint8 mode, quint8 control, quint8 type, quint8 comm, int dur, quint8 bDur,
                   quint8 nrSeq, QString phrase, QString ip):
    m_experimentMode(mode), m_controlMode(control), m_type(type), m_externalComm(comm),
    m_stimulationDuration(dur), m_breakDuration(bDur), m_nrSequences(nrSeq),
    m_desiredPhrase(phrase), m_externalAddress(ip)
{

}

Paradigm::Paradigm():
    m_experimentMode(operation_mode::CALIBRATION), m_controlMode(control_mode::SYNC), m_type(paradigm_type::ERP), m_externalComm(external_comm::DISABLED),
    m_stimulationDuration(100), m_breakDuration(100), m_nrSequences(10), m_desiredPhrase("12345"), m_externalAddress("127.0.0.1")
{

}

QVariant Paradigm::toVariant() const
{
    QVariantMap map;

    map.insert("experimentMode", m_experimentMode);
    map.insert("controlMode", m_controlMode);
    map.insert("externalComm", m_externalComm);
    map.insert("paradigmType", m_type);
    map.insert("stimulationDuration", m_stimulationDuration);
    map.insert("breakDuration", m_breakDuration);
    map.insert("nrSequences", m_nrSequences);
    map.insert("desiredPhrase", m_desiredPhrase);
    map.insert("ip", m_externalAddress);
    return map;
}

void Paradigm::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    m_experimentMode = map.value("experimentMode").toInt();
    m_controlMode = map.value("controlMode").toInt();
    if(map.value("externalComm").isNull())
    {
        m_externalComm = external_comm::DISABLED;
    }
    else
    {
        m_externalComm = map.value("externalComm").toInt();
    }
    m_type = map.value("paradigmType").toInt();
    m_stimulationDuration = map.value("stimulationDuration").toInt();
    m_breakDuration = map.value("breakDuration").toInt();
    m_nrSequences = map.value("nrSequences").toInt();
    m_desiredPhrase = map.value("desiredPhrase").toString();
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

quint8 Paradigm::controlMode() const
{
    return m_controlMode;
}

void Paradigm::setControlMode(const int t_controlMode)
{
    m_controlMode = t_controlMode;
}

quint8 Paradigm::externalComm() const
{
    return m_externalComm;
}

void Paradigm::setExternalComm(const quint8 &externalComm)
{
    m_externalComm = externalComm;
}

QString Paradigm::externalAddress() const
{
    return m_externalAddress;
}

void Paradigm::setExternalAddress(const QString &externalAddress)
{
    m_externalAddress = externalAddress;
}

Paradigm::~Paradigm()
{
    
}
