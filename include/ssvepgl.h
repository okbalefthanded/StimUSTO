#ifndef SSVEPGL_H
#define SSVEPGL_H
//
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QWidget>
#include <QTimer>
#include <QUdpSocket>
#include <QLabel>
#include <QTimer>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
//
#include "randomflashsequence.h"
#include "ssvep.h"
//
class SsvepGL : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    SsvepGL(SSVEP paradigm);
    ~SsvepGL();

    void setFrequencies(QString t_freqs);
    void setFlickeringMode(int t_mode);
    void setStimulationDuration(float t_stimDuration);
    void setBreakDuration(int t_brkDuration);
    void setSequence(int t_sequence);
    void setFeedbackPort(int t_port);

    // QOpenGLWindow interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

signals:
    void markerTag(uint64_t ovStimulation);

private slots:

    void startTrial();
    void preTrial();
    //    void feedback();
    void postTrial();
    void Flickering();
    //    void pauseFlashing();
    //    void receiveFeedback();
    void update();

public slots:

    void wait(int millisecondsToWait);
    //    void create_layout();
    //    void refresh_layout();
    void initElements();
    //
    void sendMarker(uint64_t ovStimulation){
        emit markerTag(ovStimulation);
    }

private:
    //
    //    bool isTarget();
    void highlightTarget();
    void refreshTarget();

    // helpers
//    static const qint8 VERTICES_PER_TRIANGLE = 3;
//    static const qint8 POINTS_PER_SQUARE = 4;
//    static const qint8 INDICES_PER_SQUARE = 6;
//    static const qint8 TRIANGLES_PER_SQUARE = 2;
//    static const int TUPLESIZE  = 3;
//    static const int MAX_ELEMENTS = 4;

    bool m_firstRun = true;
    int m_preTrialCount=0;
    int m_preTrialWait;
    int m_currentFlicker=0;
    int m_state;
    int m_flickeringMode;
    int m_nrElements;
    //= MAX_ELEMENTS;
    QLabel *m_textRow;
    QList<double> m_frequencies;
    float m_stimulationDuration;
    int m_stimulationSequence;
    int m_breakDuration;

    quint16 m_feedbackPort = 12345;

    QUdpSocket *m_feedbackSocket;
    // Timers
    QTimer *m_preTrialTimer;

    RandomFlashSequence *m_flickeringSequence;

    // OpenGL State Information
    QOpenGLBuffer m_vertexBuffer; //vbo
    QOpenGLBuffer m_colorBuffer; //vbo

    QOpenGLVertexArrayObject m_vaObject; //vao
    QOpenGLShaderProgram *m_programShader;

    int m_index;
    //
    QVector <QVector <int>> m_flicker;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_colors;
    QVector<int> m_vindices;

};

#endif // SSVEPGL_H
