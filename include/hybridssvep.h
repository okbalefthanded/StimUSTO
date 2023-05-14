#ifndef HYBRIDSSVEP_H
#define HYBRIDSSVEP_H
//
#include <QUdpSocket>
#include <QTcpSocket>
#include <QPropertyAnimation>
//
#include "externComm.h"
#include "ssvepstimulation.h"
#include "paradigm.h"
#include "ssvep.h"
#include "doublessvep.h"
//
class HybridSSVEP : public QObject
{
    Q_OBJECT

public:
    explicit HybridSSVEP(DoubleSSVEP *hybridStimulation);
    explicit HybridSSVEP(DoubleSSVEP *hybridStimulation, SSVEPstimulation *FirstStim, SSVEPstimulation *SecondStim);

    ~HybridSSVEP();
    void initFirstSpeller(SSVEP *ssvep);
    void initSecondSpeller(SSVEP *ssvep);

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
    float m_FirstCorrect = 0;
    float m_SecondCorrect = 0;
    bool m_switchStimulation = true;
    //
    quint16 m_robotPort = 12347;
    QString m_hybridCommand  = "";
    QString m_robotFeedback  = "";
    QString m_FirstFeedback  = "";
    QString m_SecondFeedback = "";
    //
    DoubleSSVEP *m_hybridStimulation;
    SSVEPstimulation *m_FirstSpeller;
    SSVEPstimulation *m_SecondSpeller;
    QTcpSocket *m_robotSocket;
    ExternComm *m_externComm;
    //
    QPropertyAnimation *m_FirstAnimation;
    QPropertyAnimation *m_SecondAnimation;

};


#endif // HYBRIDSSVEP_H
