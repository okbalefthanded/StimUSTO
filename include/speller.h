#ifndef SPELLER_H
#define SPELLER_H
//
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QElapsedTimer>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QColor>
#include <QDialog>
//
#include "ui_spellerform.h"
//
#include "randomflashsequence.h"
#include "externComm.h"
#include "matrixlayout.h"
#include "erp.h"
#include "utils.h"
#include "logger.h"
//

/*
namespace Ui {
class Speller;
}
*/
// class Speller : public QWidget, protected Ui::SpellerForm
class Speller : public QDialog, protected Ui::SpellerForm
{
    Q_OBJECT

public:
    explicit Speller(QWidget *parent = nullptr);
    Speller(quint16 t_port);
    ~Speller();

    ERP *erp() const;
    void setERP(ERP *erp);
    QString *desiredPhrase() const;
    bool presentFeedback() const;
    void setDesiredPhrase(const QString &t_desiredPhrase);
    void setTimers(int t_stimulation, int t_isi);
    void setPresentFeedback(bool t_do);
    void setFeedbackPort(quint16 t_port);
    virtual void showFeedback (QString command, bool correct); // used for HybridStimulation
    int getCurrentTarget();

    friend class Hybrid;
    friend class HybridStimulation;
    friend class DoubleERP;
    friend class HybridERP;

    QString getDesiredPhrase() const;

signals:
    void markerTag(uint64_t t_ovStimulation);
    void slotTerminated();

protected slots:

    virtual void preTrial();
    virtual void startTrial();
    virtual void feedback();
    virtual void postTrial();
    virtual void startFlashing();
    virtual void pauseFlashing();
    virtual void receiveFeedback();
    virtual void createLayout();
    virtual void refreshLayout();

public slots:

    void sendMarker(uint64_t t_ovStimulation){
        emit markerTag(t_ovStimulation);
    }

protected:

    void showWindow();
    void initLogger();
    void initTimers();
    void initFeedbackSocket();
    void endInit();
    bool isTarget();
    bool isTarget(int t_stim);
    bool isAsync();
    bool Correct();
    virtual void highlightTarget();
    virtual void refreshTarget();

    virtual void sendStimulationInfo();
    void switchStimulationTimers();

    void experimentStart();
    void startPreTrial();
    void endPreTrial();
    void postTrialEnd();
    void trialEnd();

    virtual int getnElements();
    // void externalCommunication();
    void fillFeedBackMap(QPixmap *map, QColor t_mapColor, QColor t_textColor, QString text="");
    // Ui::Speller *ui;

    QList<QPixmap> m_multStimuli = {QPixmap(":/images/bennabi_face_magenta.png"),
                                    // QPixmap(":/images/bennabi_face_blue.png"),
                                    QPixmap(":/images/bennabi_face_green.png"),
                                    // QPixmap(":/images/bennabi_face_yellow.png"),
                                    QPixmap(":/images/bennabi_face_red.png"),
                                    QPixmap(":/images/bennabi_face_orange.png")};

    int m_rows;
    int m_cols;
    int m_nrElements;
    int m_matrixWidth;
    int m_matrixHeight;
    int m_currentStimulation = 0;
    int m_currentLetter = 0;
    int m_currentTarget = 0;
    int m_trials = 0;
    float m_correct = 0;

    bool m_stimulus = true;
    bool m_presentFeedback = true;
    bool m_stateFinished = false;
    bool m_running = true;
    bool isCorrect = false;
    int  m_state;
    //
    ERP *m_ERP;
    //
    int m_preTrialWait = 1; // 2 seconds
    int m_preTrialCount = 0;
    quint16 m_feedbackPort = 12345;

    QString m_desiredPhrase = "";
    QString m_text = "";
    //    QImage *m_faceStimuli;
    QList<QString> m_presentedLetters;
    //    QList<QImage> m_icons;
    //    QList<QWidget> m_icons;
    //     QList<QLabel*> m_icons;
    QList<QPixmap> m_icons;
    MatrixLayout *m_mLayout;
    QLabel *m_element;
    QColor m_highlightColor = Qt::white; // Qt::yellow;
    QColor m_correctColor   = Qt::yellow; //Qt:green
    QColor m_incorrectColor = Qt::blue;
    QPixmap m_highlight;
    // Timers
    QTimer *m_preTrialTimer;
    QTimer *m_stimTimer;
    QTimer *m_isiTimer;
    //
    QLabel *m_textRow;
    QUdpSocket *m_feedbackSocket;
    RandomFlashSequence *m_flashingSequence;

    // external communication
    // QString m_command = "";
    // quint16 m_machinePort = 12347;
    // QTcpSocket *m_machineSocket;
    ExternComm *m_externComm;

    // logger
    Logger *log;
};

#endif // SPELLER_H
