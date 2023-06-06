#ifndef SSVEPSTIMULATION_H
#define SSVEPSTIMULATION_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QUdpSocket>
#include <QTimer>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QDebug>
#include <QTcpSocket>
//
#include "randomflashsequence.h"
#include "ssvep.h"
#include "logger.h"
#include "externComm.h"

class SSVEPstimulation : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit SSVEPstimulation(SSVEP *paradigm, int t_port);
    SSVEPstimulation();
    ~SSVEPstimulation();

    void setFrequencies(QString t_freqs);
    void setFeedbackPort(int t_port);

    SSVEP *ssvep() const;
    void setSsvep(SSVEP *ssvep);

    friend class Hybrid;
    friend class HybridSSVEP;
    friend class HybridStimulation;

    bool isCorrect() const;
    bool presentFeedback() const;
    void setPresentFeedback(bool presentFeedback);

    int externalFeedback() const;
    void setExternalFeedback(int newExternalFeedback, QColor color=Qt::white);

    bool showExternalFeedback() const;
    void setShowExternalFeedback(bool newShowExternalFeedback);

// QOpenGLWindow interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

signals:
    void markerTag(uint64_t ovStimulation);
    void slotTerminated();

// private slots:
protected slots:

    void startTrial();
    void preTrial();
    void feedback();
    void postTrial();
    void postTrialEnd();
    void Flickering();
    // void pauseFlashing();
    // void receiveFeedback();
    void update();

    virtual void initElements();
    virtual void initCenters();
    virtual void initCircles();
    virtual void initColors();
    virtual void initIndices();

public slots:

    //    void create_layout();
    void refreshCircles();

    void sendMarker(uint64_t ovStimulation){
        emit markerTag(ovStimulation);
    }

protected slots:
    void receiveFeedback();

protected:
    void externalCommunication();
    void initExternalSocket();

// private:
    //    bool isTarget();
    void highlightTarget();
    void refreshTarget();
    void highlightFeedback(QVector3D feedbackColor, int feedbackIndex);
    void refresh(int feedbackIndex);
    void initLogger();
    void scheduleRedraw();
    void renderText();
    void renderFeedBackText();
    void setVertex(int t_index, float x, float y, float z);

    bool m_firstRun = true;
    bool m_stateFinished = true;
    bool m_presentFeedback = true;
    bool m_receivedFeedback = false;
    int m_preTrialCount=1;
    int m_preTrialWait;
    int m_currentFlicker=0;
    int m_state;
    int m_trials = 0;
    float m_correct = 0;
    int m_externalFeedback = 0;
    QColor m_externalFeedbackColor = Qt::white;
    bool m_showExternalFeedback = false;

    int m_lostFrames = 0;
    //
    SSVEP *m_ssvep;
    QList<double> m_frequencies;

    quint16 m_feedbackPort = 12345;
    QString m_sessionFeedback;

    QUdpSocket *m_feedbackSocket;
    // Timers
    QTimer *m_preTrialTimer;

    RandomFlashSequence *m_flickeringSequence;

    // logger
    Logger *log;

    // OpenGL State Information
    QOpenGLBuffer m_vertexBuffer; //vbo
    QOpenGLBuffer m_colorBuffer; //vbo

    QOpenGLVertexArrayObject m_vaObject; //vao
    QOpenGLShaderProgram *m_programShader;

    int m_index;
    int m_vertexPerCircle;
    int tmp_t = 0;
    // QVector <QVector <int>> m_flicker;
    QVector<QVector3D> m_centerPoints;
    QVector <QVector <double> > m_flicker;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_centers;
    QVector<QVector3D> m_colors;
    QVector<int> m_vindices;
    QVector<int> m_centerindices;

    // external communication
    ExternComm *m_externComm;
    // QString m_hybridCommand = "";
    // quint16 m_robotPort = 12347;
    // QTcpSocket *m_robotSocket;
};

#endif // SSVEPSTIMULATION_H
