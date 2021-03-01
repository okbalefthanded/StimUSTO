#ifndef HYBRIDSTIMULATION_H
#define HYBRIDSTIMULATION_H
//
#include <QUdpSocket>
#include <QTcpSocket>
#include <QPropertyAnimation>
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
    void initExternalComm();
    void externalComm();

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
    Hybrid *m_hybridStimulaiton;
    Speller *m_ERPspeller;
    SsvepGL *m_ssvepStimulation;
    // QUdpSocket *m_robotSocket;
    QTcpSocket *m_robotSocket;
    //
    QPropertyAnimation *m_animation;
};


#endif // HYBRIDSTIMULATION_H
