#ifndef FLASHINGSPELLER_H
#define FLASHINGSPELLER_H
//
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QElapsedTimer>
#include <QUdpSocket>
//
#include "randomflashsequence.h"
#include "matrixlayout.h"

namespace Ui {
class FlashingSpeller;
}

class FlashingSpeller : public QWidget
{
    Q_OBJECT

public:
    explicit FlashingSpeller(QWidget *parent = 0);
    ~FlashingSpeller();

    void setStimulation_duration(int value);
    void setIsi(int value);
    void setNr_sequence(int value);
    void setNr_trials(int value);
    void setSpelling_mode(int value);
    void setDesired_phrase(const QString &value);
    void setSpeller_type(int value);
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


    Ui::FlashingSpeller *ui;

    int rows;
    int cols;
    int nr_elements;
    int matrix_width;
    int matrix_height;
    int currentStimulation = 0;
    int currentLetter = 0;
    int currentTarget = 0;

    bool stimulus = true;

    bool state_finished = false;
    bool running = true;
    int state;

    int stimulation_duration;
    int isi;
    int nr_sequence;
    int nr_trials;
    int spelling_mode;
    int speller_type;
    int pre_trial_wait = 2; // 2 seconds
    int pre_trial_count = 0;
    quint16 feedback_port = 12345;

    QString desired_phrase;
    QString text_row;
    QImage *face_stimuli;
    QList<QChar> presented_letters;
    MatrixLayout *Mlayout;
    // Timers
    QTimer *preTrialTimer;
    QTimer *stimTimer;
    QTimer *isiTimer;

    QLabel *textRow;
    QUdpSocket *feedback_socket;
    RandomFlashSequence *flashingSequence;

};

#endif // FLASHINGSPELLER_H
