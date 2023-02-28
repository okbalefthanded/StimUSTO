#include <QDebug>
#include "erp.h"
#include "utils.h"
#include "randomflashsequence.h"

ERP::ERP() : Paradigm (), m_flashingMode(flashing_mode::SC)
{

}

ERP::ERP(quint8 mode, quint8 control, quint8 type, quint8 comm, int dur, quint8 bDur,
         quint8 nrSeq, QString phrase,  QString ip, quint8 sType, quint8 fMode):
    Paradigm(mode, control, type, comm, dur, bDur, nrSeq, sType, phrase, ip),
    m_flashingMode(fMode)
{

}

QVariant ERP::toVariant() const
{
    QVariantMap map;

    map.insert("experimentMode", m_experimentMode);
    map.insert("controlMode", m_controlMode);
    map.insert("paradigmType", m_type);
    map.insert("externalComm", m_externalComm);
    map.insert("ip", m_externalAddress);
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
    int n_elements = 9;

    QVariantMap map = variant.toMap();
    m_experimentMode = map.value("experimentMode").toInt();
    m_controlMode = map.value("controlMode").toInt();
    if(map.value("externalComm").isNull())
    {
        m_externalComm = external_comm::DISABLED;
        m_externalAddress = "127.0.0.1"; // home sweet home
    }
    else
    {
        m_externalComm = map.value("externalComm").toInt();
        m_externalAddress = map.value("ip").toString();
    }
    m_type = map.value("paradigmType").toInt();
    m_stimulationDuration = map.value("stimulationDuration").toInt();
    m_breakDuration = map.value("breakDuration").toInt();
    m_nrSequences = map.value("nrSequences").toInt();

    m_desiredPhrase = map.value("desiredPhrase").toString();
    m_stimulationType = map.value("stimulationType").toInt();

    if (m_stimulationType == speller_type::AUDITORY)
    {
        n_elements = 5;
    }
    else if (m_stimulationType >= speller_type::SMALL || m_stimulationType <= speller_type::SMALL_ICFACE)
    {
        n_elements = 6;
    }

    if(m_desiredPhrase.isEmpty())
    {
        RandomFlashSequence *randomPhrase;

        if (m_experimentMode == operation_mode::CALIBRATION)
        {
            // randomPhrase = new RandomFlashSequence(9, 4);
            randomPhrase = new RandomFlashSequence(n_elements, 4);
            // randomPhrase = new RandomFlashSequence(9, 2);
            // randomPhrase = new RandomFlashSequence(9, 1);

            // RandomFlashSequence *randomPhrase = new RandomFlashSequence(9, 2);
            // RandomFlashSequence *randomPhrase = new RandomFlashSequence(9, 4);
            // RandomFlashSequence *randomPhrase = new RandomFlashSequence(9, 3);
        }
        else
        {
            //  randomPhrase = new RandomFlashSequence(9, 6);
            randomPhrase = new RandomFlashSequence(n_elements, 4);
        }

        m_desiredPhrase = randomPhrase->toString();
    }

    // qDebug()<< n_elements << m_desiredPhrase;
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
