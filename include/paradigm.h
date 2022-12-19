#ifndef PARADIGM_H
#define PARADIGM_H
//
#include "serializable.h"
#include "utils.h"
//
class Paradigm : public Serializable
{


public:
    explicit Paradigm(quint8 mode, quint8 control, quint8 comm, quint8 type, int dur,
                      quint8 bDur, quint8 nrSeq, quint8 stype, QString phrase, QString ip);
    explicit Paradigm();
    ~Paradigm();

    QVariant toVariant()  const override;
    void fromVariant(const QVariant& variant)  override;

    int experimentMode() const;
    void setExperimentMode(int t_experimentMode);

    int type() const;
    void setType(int t_type);

    int stimulationDuration() const;
    void setStimulationDuration(int t_stimulationDuration);

    int breakDuration() const;
    void setBreakDuration(int t_breakDuration);

    int nrSequences() const;
    void setNrSequences(int t_nrSequences);

    QString desiredPhrase() const;
    void setDesiredPhrase(const QString &t_desiredPhrase);

    quint8 controlMode() const;
    void setControlMode(const int t_controlMode);

    quint8 externalComm() const;
    void setExternalComm(const quint8 &externalComm);

    QString externalAddress() const;
    void setExternalAddress(const QString &externalAddress);

    quint8 stimulationType() const;
    void setStimulationType(quint8 newStimulationType);

protected:
    quint8 m_experimentMode;
    quint8 m_controlMode;
    quint8 m_type;
    quint8 m_externalComm;
    int m_stimulationDuration;
    int m_breakDuration; // isi for ERP, break for SSVEP
    quint8 m_nrSequences;
    quint8 m_stimulationType;
    QString m_desiredPhrase;
    QString m_externalAddress;

};

#endif // PARADIGM_H
