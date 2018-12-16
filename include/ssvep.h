#ifndef SSVEP_H
#define SSVEP_H
//
#include "paradigm.h"
//
class SSVEP : public Paradigm
{
public:
    explicit SSVEP();
    explicit SSVEP(quint8 mode, quint8 type, int dur, quint8 bDur, quint8 nrSeq, QString phrase, quint8 nElements,
                   QString frequnecies);

    QVariant toVariant() const override;
    void fromVariant(const QVariant& variant) override;

    quint8 nrElements() const;
    void setNrElements(const quint8 &nrElements);

    QString frequencies() const;
    void setFrequencies(const QString &frequencies);

private:
    quint8 m_nrElements;
    QString m_frequencies;

};

#endif // SSVEP_H
