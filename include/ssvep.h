#ifndef SSVEP_H
#define SSVEP_H
//
#include "paradigm.h"
//
class SSVEP : public Paradigm
{

public:
    explicit SSVEP();
    explicit SSVEP(quint8 mode, quint8 control, quint8 type, int dur, quint8 bDur, quint8 nrSeq, QString phrase, quint8 nElements,
                   QString frequnecies, quint8 stimulationMode);

    QVariant toVariant() const override;
    void fromVariant(const QVariant& variant) override;

    quint8 nrElements() const;
    void setNrElements(const quint8 &nrElements);

    QString frequencies() const;
    void setFrequencies(const QString &frequencies);

    quint8 stimulationMode() const;
    void setStimulationMode(const quint8 &stimulationMode);

private:
    quint8 m_nrElements;
    QString m_frequencies;
    quint8 m_stimulationMode;

};

#endif // SSVEP_H
