#ifndef SSVEPGL_H
#define SSVEPGL_H
//
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
//
#include "randomflashsequence.h"
#include "ssvep.h"
#include "logger.h"
//
class SsvepGL : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    SsvepGL(SSVEP *paradigm, int t_port);
    ~SsvepGL();

    void setFrequencies(QString t_freqs);
    void setFeedbackPort(int t_port);

    SSVEP *ssvep() const;
    void setSsvep(SSVEP *ssvep);

    friend class Hybrid;
    friend class HybridStimulation;

    // QOpenGLWindow interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

signals:
    void markerTag(uint64_t ovStimulation);
    void slotTerminated();

private slots:

    void startTrial();
    void preTrial();
    void feedback();
    void postTrial();
    void Flickering();
    //    void pauseFlashing();
    void receiveFeedback();
    void update();

    void initElements();
    void initRects();
    void initColors();
    void initIndices();

public slots:

    //    void create_layout();
    //    void refresh_layout();

    void sendMarker(uint64_t ovStimulation){
        emit markerTag(ovStimulation);
    }

private:
    //    bool isTarget();
    void highlightTarget();
    void refreshTarget();
    void highlightFeedback(QVector3D feedbackColor, int feebdackIndex);
    void refresh(int feedbackIndex);
    void initLogger();
    void scheduleRedraw();

    bool m_firstRun = true;
    bool m_stateFinished = true;
    int m_preTrialCount=1;
    int m_preTrialWait;
    int m_currentFlicker=0;
    int m_state;
    int m_trials = 0;
    float m_correct = 0;

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
    //
    // QVector <QVector <int>> m_flicker;
    QVector <QVector <double>> m_flicker;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_colors;
    QVector<int> m_vindices;


};

#endif // SSVEPGL_H
