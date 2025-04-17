#ifndef SSVEP_H
#define SSVEP_H
//
#include "paradigm.h"
//
class SSVEP : public Paradigm
{

public:
    explicit SSVEP();
    explicit SSVEP(QString mode, QString control,
                   QString type, QString comm,
                   int dur, quint8 bDur,
                   quint8 nrSeq, QString sType,
                   QString phrase, QString ip,
                   quint8 nElements, QString frequnecies,
                   QString stimulationMode);

    QVariant toVariant() const override;
    void fromVariant(const QVariant& variant) override;

    quint8 nrElements() const;
    void setNrElements(const quint8 &nrElements);

    QString frequencies() const;
    void setFrequencies(const QString &frequencies);

    QString stimulationMode() const;
    void setStimulationMode(const QString &stimulationMode);

private:
    quint8 m_nrElements;
    QString m_frequencies;
    QString m_stimulationMode;
    // quint8 m_stimShape;

};

#endif // SSVEP_H
