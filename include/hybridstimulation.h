#ifndef HYBRIDSTIMULATION_H
#define HYBRIDSTIMULATION_H
//
#include "ssvepgl.h"
#include "speller.h"
#include "paradigm.h"
#include "erp.h"
#include "ssvep.h"
#include "hybrid.h"
//
class HybridStimulation : public QObject
{
    Q_OBJECT

public:
    explicit HybridStimulation(Hybrid *hybridStimulation);
    explicit HybridStimulation(Hybrid *hybridStimulation, Speller *ERPspeller,
                               SsvepGL *ssvepGL);
    ~HybridStimulation();
    void initERPspeller(ERP *erp);
    void initSSVEP(SSVEP *ssvep);

signals:
    void experimentEnd();
private slots:
    void hybridPreTrial();
    void startTrial();
    //    void hybridFeedback();
    void hybridPostTrial();
    void switchState();
    void swichStimWindows();


private:
    int m_hybridState = trial_state::PRE_TRIAL;
    int m_trials;
    int m_currentTrial = 0;
    bool m_switchStimulation = true;
    Hybrid *m_hybridStimulaiton;
    Speller *m_ERPspeller;
    SsvepGL *m_ssvepStimulation;
};


#endif // HYBRIDSTIMULATION_H
