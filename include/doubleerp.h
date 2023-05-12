#ifndef DOUBLEERP_H
#define DOUBLEERP_H
#include "serializable.h"
#include "speller.h"
#include "paradigm.h"
#include "erp.h"
//
class DoubleERP : public Paradigm
{
    //Q_OBJECT

public:
    explicit DoubleERP();
    explicit DoubleERP(ERP *first, ERP *second);
    ~DoubleERP();

    QVariant toVariant()  const override;
    void fromVariant(const QVariant& variant)  override;

    friend class HybridERP;
    friend class ConfigPanel;

private:
    QString getRandomSequence(ERP *paradigm, int repetitions);
    ERP *m_1stParadigm;
    ERP *m_2ndParadigm;

};

#endif // DOUBLEERP_H
