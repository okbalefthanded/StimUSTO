#ifndef HYBRIDGRIDSTIMULATION_H
#define HYBRIDGRIDSTIMULATION_H
//
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QElapsedTimer>
#include <QTimer>
#include <QUdpSocket>
#include <QGridLayout>
//
#include "randomflashsequence.h"
//
namespace Ui {
class HybridGridStimulation;
}

class HybridGridStimulation : public QWidget
{
    Q_OBJECT

public:
    explicit HybridGridStimulation(QWidget *parent = 0);
    ~HybridGridStimulation();

    // ERP stimulation
    void setERPStimulationDuration(int value);
    void setIsi(int value);
    void setERPNrSequence(int value);
    void setERPNrtrials(int value);
    void setOperationMode(int value);
    void setDesiredSequence(const QString &value);
    void setERPStimulationType(int value);
    // SSVEP stimulation
    void setFrequencies(QString freqs);
    void setFlickeringMode(int mode);
    void setSSVEPStimulationDuration(int stimDuration);
    void setBreakDuration(int brkDuration);
    void setSSVEPSequence(int sequence);
    // general
    void setFeedbackPort(quint16 value);

signals:
    void markerTag(uint64_t ovStimulation);

private slots:

    void startTrial();
    void pre_trial();
    void feedback();
    void post_trial();
    void startFlashing();
    void pauseFlashing();
    void Flickering();
    void receiveFeedback();


public slots:

    void wait(int millisecondsToWait);
    void create_layout();
    void refresh_layout();

    //
    void sendMarker(uint64_t ovStimulation){
        emit markerTag(ovStimulation);
    }

private:

    bool isTarget();
    void highlightTarget();
    void refreshTarget();

    Ui::HybridGridStimulation *ui;

    int rows;
    int cols;
    int nrERPElements;
    int nrSSVEPElements;
    int nrRows;
    int nrCols;
    int matrix_width;
    int matrix_height;
    int currentStimulation = 0;
    int currentLetter = 0;
    int currentTarget = 0;

    bool stimulus = true;

    bool state_finished = false;
    bool running = true;
    int state;

    int ERPstimulationDuration;
    int SSVEPStimulationDuration;
    int isi;
    int nrERPSequence;
    int nrERPTrials;
    int operationMode;
    int ERPStimulationType;
    int pre_trial_wait = 2; // 2 seconds
    int pre_trial_count = 0;
    quint16 feedbackPort = 12345;

    QString desiredSequence;
    QString text_row;
    QImage *faceStimuli;
    QList<QString> presentedCommands;
    QList<float> frequencies;

    // Timers
    QTimer *preTrialTimer;
    QTimer *stimTimer;
    QTimer *isiTimer;
    //
    QLabel *textRow;
    QUdpSocket *feedbackSocket;
    RandomFlashSequence *ERPFlashingSequence;
    RandomFlashSequence *SSVEPFlickeringSequence;

    // Layouts
    QGridLayout *ERPLayout;
    QGridLayout *SSVEPLayout;

};

#endif // HYBRIDGRIDSTIMULATION_H
