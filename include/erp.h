#ifndef ERP_H
#define ERP_H
//
#include "paradigm.h"
#include "serializable.h"
#include "utils.h"
//
class ERP : public Paradigm
{


public:
    explicit ERP();
    explicit ERP(quint8 mode, quint8 control, quint8 type, quint8 comm, int dur, quint8 bDur, quint8 nrSeq, QString phrase, quint8 sType,
                 quint8 fMode);

    QVariant toVariant() const override;
    void fromVariant(const QVariant& variant) override;

    quint8 stimulationType() const;
    void setStimulationType(const quint8 &t_stimulationType);

    quint8 flashingMode() const;
    void setFlashingMode(const quint8 &t_flashingMode);

private:
    quint8 m_stimulationType;
    quint8 m_flashingMode;
};
#endif // ERP_H
