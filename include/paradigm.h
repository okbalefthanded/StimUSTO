#ifndef PARADIGM_H
#define PARADIGM_H
//
#include "serializable.h"
//
class Paradigm : public Serializable
{


public:
    explicit Paradigm(QString mode, QString control,
                      QString type, QString comm,
                      int dur, quint8 bDur,
                      quint8 nrSeq, QString stype,
                      QString phrase, QString ip);
    explicit Paradigm();
    ~Paradigm();

    QVariant toVariant()  const override;
    QVariant toVariantSave()  const override;
    void fromVariant(const QVariant& variant)  override;

    QString experimentMode() const;
    void setExperimentMode(QString t_experimentMode);

    QString type() const;
    void setType(QString t_type);

    int stimulationDuration() const;
    void setStimulationDuration(int t_stimulationDuration);

    int breakDuration() const;
    void setBreakDuration(int t_breakDuration);

    int nrSequences() const;
    void setNrSequences(int t_nrSequences);

    QString desiredPhrase() const;
    void setDesiredPhrase(const QString &t_desiredPhrase);

    QString controlMode() const;
    void setControlMode(const QString t_controlMode);

    QString externalComm() const;
    void setExternalComm(const QString &externalComm);

    QString externalAddress() const;
    void setExternalAddress(const QString &externalAddress);

    QString stimulationType() const;
    void setStimulationType(QString newStimulationType);

    QVariantMap map() const;
    void setMap(QVariantMap newMap);

    static const QVector<QString> basicSettings;

protected:
    QString m_experimentMode;
    QString m_controlMode;
    QString m_type; // paraidgm type
    QString m_externalComm;
    int m_stimulationDuration;
    int m_breakDuration; // isi for ERP, break for SSVEP
    quint8 m_nrSequences;
    QString m_stimulationType;
    QString m_desiredPhrase;
    QString m_externalAddress;
    //
    QVariantMap m_map;

};

#endif // PARADIGM_H
