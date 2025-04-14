#ifndef STIMULATION_H
#define STIMULATION_H
//
#include <QUdpSocket>
#include <QObject>
//
#include "randomflashsequence.h"
#include "externComm.h"
#include "paradigm.h"

// Base stimulation class
class Stimulation : public QObject
{
        Q_OBJECT

public:
    Stimulation();
    ~Stimulation();

    Paradigm *getSettings() const;
    void setSettings(Paradigm *newSettings);

protected:
    void initTimers(){};
    void initFeedbackSocket();

    virtual void experimentStart() {};
    virtual void startPreTrial(){};
    virtual void endPreTrial(){};
    virtual void postTrialEnd(){};
    virtual void trialEnd(){};

    virtual void highlightTarget(){};
    virtual void refreshTarget(){};

signals:
    void markerTag(uint64_t t_ovStimulation);
    void slotTerminated();

public slots:

    void sendMarker(uint64_t t_ovStimulation){
        emit markerTag(t_ovStimulation);
    }

protected slots:
    virtual void preTrial(){};
    virtual void startTrial(){};
    virtual void postTrial(){};
    virtual void startFlashing(){};
    virtual void pauseFlashing(){};
    virtual void feedback(){};
    void receiveFeedback();

protected:
    Paradigm *m_settings;

    bool m_stimulus = true;
    bool m_presentFeedback = true;
    bool m_stateFinished = false;
    int m_state;
    int m_trials = 0;
    float m_correct = 0;
    // feedback object
    quint16 m_feedbackPort = 12345;
    QUdpSocket *m_feedbackSocket;
    QString m_sessionFeedback = "";
    bool m_receivedFeedback = false;
    // Timers
    QTimer *m_preTrialTimer;
    //
    RandomFlashSequence *m_flashingSequence;
    // external communication
    ExternComm *m_externComm;
};

#endif // STIMULATION_H
