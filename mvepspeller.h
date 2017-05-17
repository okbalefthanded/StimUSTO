#ifndef MVEPSPELLER_H
#define MVEPSPELLER_H
//
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QElapsedTimer>
#include <QUdpSocket>
#include <QGraphicsView>
#include <QList>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
//
#include "randomflashsequence.h"
#include "matrixlayout.h"
#include "motionitem.h"
//
namespace Ui {
class mVEPSpeller;
}

class mVEPSpeller : public QWidget
{
    Q_OBJECT

public:
    explicit mVEPSpeller(quint8 spellerType, QWidget *parent = 0);
    ~mVEPSpeller();

    void setStimulation_duration(int value);
    void setIsi(int value);
    void setNr_sequence(int value);
    void setNr_trials(int value);
    void setSpelling_mode(int value);
    void setDesired_phrase(const QString &value);
    void setSpeller_type(quint16 value);
    void setFeedbackPort(quint16 value);

signals:
    void markerTag(uint64_t ovStimulation);
    void timeOutSignal();

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

    Ui::mVEPSpeller *ui;

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

    quint16 stimulation_duration;
    quint16 isi;
    quint16 nr_sequence;
    quint16 nr_trials;
    quint8 spelling_mode;
    quint16 speller_type;
    quint8 pre_trial_wait = 2; // 2 seconds
    quint8 pre_trial_count = 0;
    quint16 feedback_port = 12345;

    QString desired_phrase;
    QString text_row;
    QImage face_stimuli;
    MatrixLayout *Mlayout;
    // Timers
    QTimer *preTrialTimer;
    QTimer *stimTimer;
    QTimer *isiTimer;
    QTimeLine *animTimer;
    // Utils
    QLabel *textRow;
    QUdpSocket *feedback_socket;
    RandomFlashSequence *flashingSequence;
    // Graphics
    QGraphicsView *view;
    QList<QGraphicsItem*> itemsList;
    QGraphicsPixmapItem *image_stimuli;
    QGraphicsItemAnimation *animation;
    QGraphicsRectItem *bar_stimuli;
};

#endif // MVEPSPELLER_H
