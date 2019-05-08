#ifndef HYBRIDSTIMULATION_H
#define HYBRIDSTIMULATION_H
//
#include "ssvepgl.h"
#include "speller.h"
#include "paradigm.h"
#include "erp.h"
#include "ssvep.h"
//
class HybridStimulation
{
    Q_OBJECT

public:
    explicit HybridStimulation(ERP *erp, SSVEP *ssvep);
    ~HybridStimulation();
    void initERPspeller(ERP *erp);
    void initSSVEP(SSVEP *ssvep);

private slots:
    void hybridPreTrial();
    void hybridStartTrial();
    void hybridFeedback();
    void hybridPostTrial();


private:
    int m_hybridState;
    Speller *ERPspeller;
    SsvepGL *ssvepStimulation;
};


#endif // HYBRIDSTIMULATION_H
