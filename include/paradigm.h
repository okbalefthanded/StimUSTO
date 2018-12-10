#ifndef PARADIGM_H
#define PARADIGM_H
//
#include "serializable.h"
#include "utils.h"
//
class Paradigm : public Serializable
{
public:
    explicit Paradigm(quint8 mode, quint8 type, int dur, quint8 bDur, quint8 nrSeq, QString phrase);
    explicit Paradigm();
    ~Paradigm();

    QVariant toVariant()  const override {}
    void fromVariant(const QVariant& variant)  override {}

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

protected:
    quint8 m_experimentMode;
    quint8 m_type;
    int m_stimulationDuration;
    int m_breakDuration; // isi for ERP, break for SSVEP
    quint8 m_nrSequences;
    QString m_desiredPhrase;

};

#endif // PARADIGM_H
