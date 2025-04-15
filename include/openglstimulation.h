#ifndef OPENGLSTIMULATION_H
#define OPENGLSTIMULATION_H
//
#include <QOpenGLWindow>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QUdpSocket>
#include <QTimer>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QDebug>
#include <QTcpSocket>
#include <QElapsedTimer>
//
#include "ssvep.h"
#include "logger.h"
#include "framelogger.h"

class OpenGLStimulation : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLStimulation(SSVEP *paradigm);
    OpenGLStimulation();
    ~OpenGLStimulation();

    friend class SSVEPStimulation;

public:
    virtual void initElements();
    QVector<double> frequencies() const;
    void setFrequencies(QString t_freqs);

// QOpenGLWindow interface
    QVector<QVector<double> > flicker() const;
    void setFlicker(const QVector<QVector<double> > &newFlicker);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

protected slots:

    void update();

    virtual void initCenters();
    virtual void initFlickers();
    virtual void initColors();
    virtual void initIndices();

public slots:
    //    void create_layout();
    void refreshFlickers();


protected:

    // private:
    //    bool isTarget();
    void initFormat();
    void initIntensity();
    void highlightTarget(){};
    void highlightFeedback(QVector3D feedbackColor, int feedbackIndex);
    void refreshTarget(){};
    void refresh(int feedbackIndex);
    void initLogger(){};
    void scheduleRedraw();
    void renderText(){};
    void renderFeedBackText(int index, QString text);
    void setVertex(int t_index, float x, float y, float z);

    bool m_firstRun = true;
    bool m_stateFinished = true;
    int m_currentFlicker=0;

    int m_lostFrames = 0;
    //
    SSVEP *m_ssvep;
    QVector<double> m_frequencies;
    QColor m_externalFeedbackColor = Qt::red;

    // Timers
    QElapsedTimer *correctortimer;

    // logger
    Logger *log;
    FrameLogger *logger;

    // OpenGL State Information
    QOpenGLBuffer m_vertexBuffer; //vbo
    QOpenGLBuffer m_colorBuffer; //vbo

    QOpenGLVertexArrayObject m_vaObject; //vao
    QOpenGLShaderProgram *m_programShader;

    int m_index;
    int m_vertexPerCircle;
    double time_tmp = 0;
    // QVector <QVector <int>> m_flicker;
    QVector<QVector3D> m_centerPoints;
    QVector <QVector <double> > m_flicker;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_centers;
    QVector<QVector3D> m_colors;
    QVector<int> m_vindices;
    QVector<int> m_centerindices;
};



#endif // OPENGLSTIMULATION_H
