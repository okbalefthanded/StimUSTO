#ifndef HYBRIDERP_H
#define HYBRIDERP_H

//
#include <QUdpSocket>
#include <QTcpSocket>
#include <QPropertyAnimation>
//
#include "externComm.h"
#include "speller.h"
#include "paradigm.h"
#include "erp.h"
#include "doubleerp.h"
//
class HybridERP : public QObject
{
    Q_OBJECT

public:
    explicit HybridERP(DoubleERP *hybridStimulation);
    explicit HybridERP(DoubleERP *hybridStimulation, Speller *FirstSpeller, Speller *SecondSpeller);

    ~HybridERP();
    void initFirstSpeller(ERP *erp);
    void initSecondSpeller(ERP *erp);

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
    DoubleERP *m_hybridStimulation;
    Speller *m_FirstSpeller;
    Speller *m_SecondSpeller;
    QTcpSocket *m_robotSocket;
    ExternComm *m_externComm;
    //
    QPropertyAnimation *m_FirstAnimation;
    QPropertyAnimation *m_SecondAnimation;

};


#endif // HYBRIDERP_H
