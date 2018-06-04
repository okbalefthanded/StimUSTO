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
//
class SsvepGL : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    SsvepGL(int nrelements);
    ~SsvepGL();

    void setFrequencies(QString freqs);
    void setFlickeringMode(int mode);
    void setStimulationDuration(float stimDuration);
    void setBreakDuration(int brkDuration);
    void setSequence(int sequence);
    void setFeedbackPort(int port);

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
    static const qint8 VERTICES_PER_TRIANGLE = 3;
    static const qint8 POINTS_PER_SQUARE = 4;
    static const qint8 INDICES_PER_SQUARE = 6;
    static const qint8 TRIANGLES_PER_SQUARE = 2;
    static const int TUPLESIZE  = 3;
    static const int MAX_ELEMENTS = 4;

    bool firstRun = true;
    int pre_trial_count=0;
    int pre_trial_wait;
    int currentFlicker=0;
    int state;
    int flickering_mode;
    int nr_elements;
    //= MAX_ELEMENTS;
    QLabel *textRow;
    QList<double> frequencies;
    float stimulationDuration;
    int stimulationSequence;
    int breakDuration;

    quint16 feedbackPort = 12345;

    QUdpSocket *feedback_socket;
    // Timers
    QTimer *preTrialTimer;

    RandomFlashSequence *flickeringSequence;

    // OpenGL State Information
    QOpenGLBuffer vertexBuffer; //vbo
    QOpenGLBuffer colorBuffer; //vbo

    QOpenGLVertexArrayObject vaObject; //vao
    QOpenGLShaderProgram *programShader;

    int index;
    //
    QVector <QVector <int>> flicker;
    QVector<QVector3D> vertices;
    QVector<QVector3D> colors;
    QVector<int> vindices;

};

#endif // SSVEPGL_H
