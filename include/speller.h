#ifndef SPELLER_H
#define SPELLER_H
//
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QElapsedTimer>
#include <QUdpSocket>
//
#include "randomflashsequence.h"
#include "matrixlayout.h"
//

namespace Ui {
class Speller;
}

class Speller : public QWidget
{
    Q_OBJECT

public:
    explicit Speller(QWidget *parent = 0);
    ~Speller();
    void setStimulationDuration(int t_stimDuration);
    void setIsi(int t_isi);
    void setNrSequence(int t_nrSequence);
    void setNrTrials(int t_nrTrials);
    void setSpellingMode(int t_spellingMode);
    void setDesiredPhrase(const QString &t_desiredPhrase);
    void setSpellerType(int t_spellerType);
    void setFeedbackPort(quint16 t_feedbackPort);

signals:
    void markerTag(uint64_t t_ovStimulation);

protected slots:

    void startTrial();
    void preTrial();
    void feedback();
    void postTrial();
    virtual void startFlashing();
    void pauseFlashing();
    void receiveFeedback();
    void createLayout();
    void refreshLayout();

public slots:

    void wait(int t_millisecondsToWait);


    //
    void sendMarker(uint64_t t_ovStimulation){
        emit markerTag(t_ovStimulation);
    }

protected:

    bool isTarget();
    void highlightTarget();
    void refreshTarget();

    Ui::Speller *ui;

    int m_rows;
    int m_cols;
    int m_nrElements;
    int m_matrixWidth;
    int m_matrixHeight;
    int m_currentStimulation = 0;
    int m_currentLetter = 0;
    int m_currentTarget = 0;

    bool m_stimulus = true;

    bool m_stateFinished = false;
    bool m_running = true;
    int  m_state;

    int m_stimulationDuration;
    int m_isi;
    int m_nrSequence;
    int m_nrTrials;
    int m_spellingMode;
    int m_spellerType;
    int m_preTrialWait = 2; // 2 seconds
    int m_preTrialCount = 0;
    quint16 m_feedbackPort = 12345;

    QString m_desiredPhrase;
    QString m_text;
    QImage *m_faceStimuli;
    QList<QString> m_presentedLetters;
    MatrixLayout *m_mLayout;
    // Timers
    QTimer *m_preTrialTimer;
    QTimer *m_stimTimer;
    QTimer *m_isiTimer;

    QLabel *m_textRow;
    QUdpSocket *m_feedbackSocket;
    RandomFlashSequence *m_flashingSequence;
};

#endif // SPELLER_H
