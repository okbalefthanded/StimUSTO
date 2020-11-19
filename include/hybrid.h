#ifndef HYBRID_H
#define HYBRID_H
//
#include "serializable.h"
#include "ssvepgl.h"
#include "speller.h"
#include "paradigm.h"
#include "erp.h"
#include "ssvep.h"
//
class Hybrid : public Paradigm
{
    //Q_OBJECT

public:
    explicit Hybrid();
    explicit Hybrid(ERP *erp, SSVEP *ssvep);
    ~Hybrid();

    QVariant toVariant()  const override;
    void fromVariant(const QVariant& variant)  override;
   // void initERPspeller(ERP *erp);
   //  void initSSVEP(SSVEP *ssvep);
friend class HybridStimulation;
friend class ConfigPanel;

private:
  //   int m_hybridState;
    ERP *m_ERPparadigm;
    SSVEP *m_SSVEPparadigm;
    QString m_externalAddress;


};


#endif // HYBRID_H
