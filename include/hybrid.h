#ifndef HYBRID_H
#define HYBRID_H
//
#include "ssvepgl.h"
#include "speller.h"
#include "paradigm.h"
#include "erp.h"
#include "ssvep.h"
//
class Hybrid : public Serializable
{
    Q_OBJECT

public:
    explicit Hybrid();
    ~Hybrid();

    QVariant toVariant()  const override;
    void fromVariant(const QVariant& variant)  override;
   // void initERPspeller(ERP *erp);
   //  void initSSVEP(SSVEP *ssvep);

private:
  //   int m_hybridState;
    ERP *m_ERPparadigm;
    SSVEP *m_SSVEPparadigm;

};


#endif // HYBRID_H
