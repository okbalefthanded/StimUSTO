#ifndef SSVEP_H
#define SSVEP_H
//
#include <QWidget>
#include <QTimer>
#include <QUdpSocket>
#include <QLabel>
#include <QTimer>
//
#include "randomflashsequence.h"

namespace Ui {
class Ssvep;
}

class Ssvep : public QWidget
{
    Q_OBJECT

public:
    explicit Ssvep(QWidget *parent = 0);
    ~Ssvep();

    void setFrequencies(QString freqs);
    void setFlickeringMode(int mode);
    void setStimulationDuration(int stimDuration);
    void setBreakDuration(int brkDuration);
    void setSequence(int sequence);
    void setFeedbackPort(int port);


signals:
    void markerTag(uint64_t ovStimulation);

private slots:

    void startTrial();
    void pre_trial();
    //    void feedback();
    void post_trial();
    void Flickering();
    //    void pauseFlashing();
    //    void receiveFeedback();

public slots:

    void wait(int millisecondsToWait);
    void create_layout();
    //    void refresh_layout();

    //
    void sendMarker(uint64_t ovStimulation){
        emit markerTag(ovStimulation);
    }



private:

    //    bool isTarget();
    void highlightTarget();
    void refreshTarget();

    bool firstRun = true;
    int pre_trial_count=0;
    int pre_trial_wait=2;
    int currentFlicker=0;
    int state;
    int flickering_mode;
    int nr_elements;
    QLabel *textRow;
    QList<float> frequencies;
    int stimulationDuration;
    int stimulationSequence;
    int breakDuration;

    quint16 feedbackPort = 12345;

    Ui::Ssvep *ui;

    QUdpSocket *feedback_socket;
    // Timers
    QTimer *preTrialTimer;

    RandomFlashSequence *flickeringSequence;

};

#endif // SSVEP_H
