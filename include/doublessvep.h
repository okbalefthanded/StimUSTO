#ifndef DOUBLESSVEP_H
#define DOUBLESSVEP_H
//
#include "serializable.h"
#include "ssvepgl.h"
#include "paradigm.h"
#include "ssvep.h"
//
class DoubleSSVEP : public Paradigm
{
    //Q_OBJECT

public:
    explicit DoubleSSVEP();
    explicit DoubleSSVEP(SSVEP *first, SSVEP *second);
    ~DoubleSSVEP();

    QVariant toVariant()  const override;
    void fromVariant(const QVariant& variant)  override;

    friend class HybridStimulation;
    friend class HybridSSVEP;
    friend class ConfigPanel;

private:
    SSVEP *m_1stParadigm;
    SSVEP *m_2ndParadigm;

};

#endif // DOUBLESSVEP_H
