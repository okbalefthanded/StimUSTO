#ifndef HYBRIDSTIMULATION_H
#define HYBRIDSTIMULATION_H
//
#include <QUdpSocket>
#include <QTcpSocket>
#include <QPropertyAnimation>
//
#include "ssvepgl.h"
#include "ssvepcircle.h"
#include "ssvepdirection.h"
#include "ssvepstimulation.h"
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
    // explicit HybridStimulation(Hybrid *hybridStimulation, Speller *ERPspeller, SsvepGL *ssvepGL);
    // explicit HybridStimulation(Hybrid *hybridStimulation, Speller *ERPspeller, SsvepCircle *ssvepGL);
    // explicit HybridStimulation(Hybrid *hybridStimulation, Speller *ERPspeller, SsvepDirection *ssvepGL);
    explicit HybridStimulation(Hybrid *hybridStimulation, Speller *ERPspeller, SSVEPstimulation *ssvepGL);

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
    void hybridPostTrialEnd();
    void switchState();
    void swichStimWindows();
    void initExternalComm();
    void externalComm();
    void terminateExperiment();
    void initAnimations();

private:
    int m_hybridState = trial_state::PRE_TRIAL;
    int m_trials;
    int m_currentTrial = 0;
    float m_ERPCorrect = 0;
    float m_SSVEPCorrect = 0;
    bool m_switchStimulation = true;
    //
    quint16 m_robotPort = 12347;
    QString m_hybridCommand = "";
    QString m_robotFeedback = "";
    QString m_ERPFeedback = "";
    QString m_SSVEPFeedback = "";
    //
    Hybrid *m_hybridStimulation;
    Speller *m_ERPspeller;
    // SsvepGL *m_ssvepStimulation;
    // SsvepCircle *m_ssvepStimulation;
    // SsvepDirection *m_ssvepStimulation;
    SSVEPstimulation *m_ssvepStimulation;
    // QUdpSocket *m_robotSocket;
    QTcpSocket *m_robotSocket;
    //
    QPropertyAnimation *m_ERPanimation;
    QPropertyAnimation *m_SSVEPanimation;

};


#endif // HYBRIDSTIMULATION_H
