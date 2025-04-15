#ifndef SSVEPSTIMULATION__H
#define SSVEPSTIMULATION__H
//
#include "openglstimulation.h"
#include "stimulation.h"
#include "ssvep.h"

class SSVEPStimulation : public Stimulation
{
    Q_OBJECT

public:
    explicit SSVEPStimulation(SSVEP *t_ssvep);
    ~SSVEPStimulation();

    void setParadigm(SSVEP *t_erp);

protected slots:
    void preTrial();
    void startTrial();
    void Flickering();
    void postTrial();
    void startFlashing(){}; // TODO
    void pauseFlashing(){}; // TODO
    void feedback();
    void createLayout();

protected:
    void initTimers();
    void postTrialEnd();
    //
    OpenGLStimulation *m_ui;
    SSVEP *m_settings;
    //
    RandomFlashSequence *m_flickeringSequence;
    QTimer *m_preTrialTimer;
    int m_preTrialWait;
    bool m_firstRun = true;
    int m_preTrialCount = 1;
    int m_currentFlicker = 0;

};

#endif // SSVEPSTIMULATION__H
