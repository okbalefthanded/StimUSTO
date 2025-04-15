#ifndef VISUALSPELLER_H
#define VISUALSPELLER_H

#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QGridLayout>
//
#include "erp.h"
// #include "ui_spellerform.h"
#include "stimulation.h"
#include "logger.h"


// class VisualSpeller : public QWidget, public Stimulation, protected Ui::SpellerForm
class VisualSpeller : public Stimulation
{
    Q_OBJECT

public:
    explicit VisualSpeller(ERP *t_erp);
    ~VisualSpeller();

    // bool presentFeedback() const;
    void setDesiredPhrase(const QString &t_desiredPhrase);
    QString getDesiredPhrase() const;
    void setTimers(int t_stimulation, int t_isi);
    // void setPresentFeedback(bool t_do);
    void setFeedbackPort(quint16 t_port);
    // virtual void showFeedback (QString command, bool correct); // used for HybridStimulation
    int getCurrentTarget();

    void setParadigm(ERP *erp);

protected slots:
    void preTrial();
    void startTrial();
    void postTrial();
    void startFlashing();
    void pauseFlashing();
    void feedback();
    void createLayout();
    virtual void refreshLayout();

    //
    void initMethodMap();
    void stimulationFlash();
    void stimulationPixMap();
    void stimulationMultiPixMap();
    void stimulationMismatchFace(){};
    void pauseFlash();
    void pauseStimPixMap();


protected:
    void experimentStart();
    void startPreTrial();
    void endPreTrial();
    void postTrialEnd();
    void trialEnd();

    void sendStimulationInfo();
    void switchStimulationTimers();
    void updateStimTimer(int start, int end);
    void updateIsiTimer(int start, int end);

    QGridLayout *prepareLayout();
    QGridLayout *createLayoutEnglish();
    QGridLayout *createLayoutArabic();
    void highlightTarget();
    void refreshTarget();

    void initTimers();
    void initStimuli();
    void showWindow();
    void endInit();
    bool isTarget();
    bool isTarget(int t_stim);
    // bool isAsync();
    bool Correct();
    virtual int getnElements();
    void fillFeedBackMap(QPixmap *map, QColor t_mapColor, QColor t_textColor, QString text="");

    //
    QMap<QString, std::function<void()>> m_stimMethodMap;
    QMap<QString, std::function<void()>> m_pauseMethodMap;
    //
    ERP *m_settings;
    int m_rows;
    int m_cols;
    int m_nrElements;
    int m_matrixWidth;
    int m_matrixHeight;
    int m_currentStimulation = 0;
    int m_currentLetter = 0;
    int m_currentTarget = 0;

    int m_preTrialWait = 1; // 2 seconds
    int m_preTrialCount = 0;
    int m_delay = 0; // in ms
    //
    QWidget *m_speller;
    QString m_desiredPhrase = "";
    QString m_text = "";
    QLabel *m_textRow;
    QList<QString> m_presentedLetters;
    QList<QPixmap> m_multStimuli = {};
    QList<int> m_stimuliSet;
    QPixmap *m_stimulus;
    // QVector<QLabel *> m_pointers;
    QLabel *m_element;
    QColor m_highlightColor = Qt::white; // Qt::yellow;
    QColor m_correctColor   = Qt::yellow; //Qt:green
    QColor m_incorrectColor = Qt::blue;
    // QPixmap m_highlight;
    bool isCorrect = false;

    QTimer *m_stimTimer;
    QTimer *m_isiTimer;
    int m_guiDelay = 0;
    // speller tmp helpers
    QLayout* m_layout;
    QLayoutItem* m_item;
    // more crazy stuff

    //
    Logger *log;
};

#endif // VISUALSPELLER_H
