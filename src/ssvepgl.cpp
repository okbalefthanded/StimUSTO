//
#include <QWindow>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTime>
#include <QElapsedTimer>
#include <windows.h>
#include <QtMath>
#include <Qapplication>
//
#include "ssvepgl.h"
#include "ovtk_stimulations.h"
#include "utils.h"
//
const quint8 CALIBRATION = 0;
const quint8 COPY_MODE  = 1;
const quint8 FREE_MODE = 2;
const quint8 SINGLE  = 3;
//
const quint8 PRE_TRIAL = 0;
const quint8 STIMULUS = 1;
const quint8 POST_STIMULUS = 2;
const quint8 FEEDBACK = 3;
const quint8 POST_TRIAL = 4;
//
const uint64_t OVTK_StimulationLabel_Base = 0x00008100;
//
static const int REFRESH_RATE = 60;
//
static const QVector<QVector3D> topPoints =
{
    QVector3D(-0.10f, 0.10f, 1.0f), QVector3D(-0.75f, 0.10f, 1.0f),
    QVector3D(0.55f, 0.10f, 1.0f), QVector3D(-0.10f, 0.80f, 1.0f),
    QVector3D(-0.10f, -0.55f, 1.0f)
};
static const QVector3D cWhite = QVector3D(1.0f, 1.0f, 1.0f);
static const QVector3D cGray = QVector3D(0.25f, 0.25f, 0.25f);
static const QVector3D cRed = QVector3D(1.0f, 0.0f, 0.0f);
static const QVector3D cGreen = QVector3D(0.0f, 1.0f, 0.0f);
//
SsvepGL::SsvepGL(quint8 nrelements)
{

    qDebug()<< Q_FUNC_INFO;

    //    if( nrelements <= MAX_ELEMENTS)
    //    {
    //       // nr_elements = nrelements - 1;
    //        nr_elements = nrelements;
    //    }
    nr_elements = nrelements;

    // set vertices, vertices indeices & colors
    initElements();

    preTrialTimer = new QTimer(this);
    preTrialTimer->setTimerType(Qt::PreciseTimer);
    preTrialTimer->setInterval(1000);
    preTrialTimer->setSingleShot(true);
    connect(preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );

    feedback_socket = new QUdpSocket(this);
    feedback_socket->bind(QHostAddress::LocalHost, feedbackPort);

    state = PRE_TRIAL;

}

void SsvepGL::initializeGL()
{

    qDebug()<< Q_FUNC_INFO;

    // Initialize OpenGL Backend
    initializeOpenGLFunctions();
    index = 0;
    // Set global information
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_FRAMEBUFFER_SRGB);


    if(  QGuiApplication::screens().size() == 2)
    {
        this->setScreen(QGuiApplication::screens().last());
        this->showFullScreen();
    }

    // set vertices, vertices indeices & colors
    // initElements();

    // static const int samplesLength = REFRESH_RATE * (stimulationDuration);

    flicker.resize(frequencies.size());
    for (int i=0; i < frequencies.size(); ++i)
    {
        flicker[i] = utils::gen_flick(frequencies[i], REFRESH_RATE, stimulationDuration);
    }

    // Application-specific initialization
    {
        // Create shaders (Do not release until VAO is created)
        programShader = new QOpenGLShaderProgram();
        programShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shaders/sh_v.vert");
        programShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shaders/sh_f.frag");
        programShader->link();
        programShader->bind();

        // Create buffer (Do not release until VAO is created)
        vertexBuffer.create();
        vertexBuffer.bind();
        vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        vertexBuffer.allocate(vertices.data(), vertices.count() * sizeof(QVector3D)); //

        colorBuffer.create();
        colorBuffer.bind();
        colorBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
        colorBuffer.allocate(colors.data(), colors.count() * sizeof(QVector3D));

        // Create Vertex Array Object
        vaObject.create();

        vaObject.bind();
        vertexBuffer.bind();
        programShader->enableAttributeArray(0);
        programShader->setAttributeBuffer(0, GL_FLOAT, 0, TUPLESIZE, 0);
        colorBuffer.bind();
        programShader->enableAttributeArray(1);
        programShader->setAttributeBuffer(1, GL_FLOAT, 0, TUPLESIZE, 0);
        vaObject.release();

        // Release (unbind) all
        vaObject.release();
        vertexBuffer.release();
        colorBuffer.release();
        programShader->release();
    }
}

void SsvepGL::resizeGL(int w, int h)
{
    //TODO
    //    (void)w;
    //    (void)h;
    initElements();
}

void SsvepGL::paintGL()
{
    //    qDebug()<< Q_FUNC_INFO;
    // clear
    glClear(GL_COLOR_BUFFER_BIT);
    int nVertices = VERTICES_PER_TRIANGLE * (nr_elements) *TRIANGLES_PER_SQUARE;
    // Render using our shader
    programShader->bind();
    {
        vaObject.bind();
        // nr_triangles = 3 * Nr_elments * 2 (3 : vertices per triangle), (2: 2 triangles per rectangle)
        glDrawElements(GL_TRIANGLES, nVertices, GL_UNSIGNED_INT, vindices.data());
        vaObject.release();
    }
    programShader->release();

}

void SsvepGL::startTrial()
{

    qDebug()<< "[TRIAL START]" << Q_FUNC_INFO;

    if (state == PRE_TRIAL)
    {
        pre_trial();
    }
    if (state == STIMULUS)
    {
        Flickering();
    }
    if (state == POST_TRIAL)
    {
        post_trial();
    }
}

void SsvepGL::pre_trial()
{

    qDebug()<< Q_FUNC_INFO;

    if(firstRun)
    {
        //  qDebug()<< nr_elements << stimulationSequence << stimulationSequence / nr_elements;
        flickeringSequence = new RandomFlashSequence(nr_elements, stimulationSequence / nr_elements);
        qDebug()<<"sequence"<<flickeringSequence->sequence;
        firstRun = false;
    }

    if (pre_trial_count == 1)
    {

        sendMarker(OVTK_StimulationId_TrialStart);

        if (flickering_mode == CALIBRATION)
        {
            //            qDebug()<< "highlightTarget";
            highlightTarget();
            //            text_row += desired_phrase[currentLetter];
            //            textRow->setText(text_row);
        }
        else if(flickering_mode == COPY_MODE)
        {
            highlightTarget();
        }
    }
    else if(pre_trial_count == 3)
    {
        refreshTarget();
    }
    //    qDebug()<< "Pre trial timer start";

    preTrialTimer->start();
    pre_trial_count++;

    if (pre_trial_count > pre_trial_wait)
    {
        refreshTarget();
        preTrialTimer->stop();
        pre_trial_count = 0;
        state = STIMULUS;

    }

}

void SsvepGL::post_trial()
{

    sendMarker(OVTK_StimulationId_TrialStop);
    //    qDebug()<< Q_FUNC_INFO << "disconnecting frameswapped to update "<<"index"<< index;
    disconnect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    initElements();
    //    currentStimulation = 0;

    index = 0;
    state = PRE_TRIAL;
    // wait
    int waitMillisec = breakDuration - pre_trial_wait * 1000;
    wait(waitMillisec);

    //    refreshTarget();

    if (currentFlicker >= flickeringSequence->sequence.size() &&
            (flickering_mode == COPY_MODE || flickering_mode == CALIBRATION))
    {
        qDebug()<< "Experiment End";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        wait(2000);
        this->close();
    }
    else
    {
        startTrial();
    }
}

void SsvepGL::Flickering()
{
    qDebug()<< Q_FUNC_INFO;

    if(index == 0)
    {
        //        qDebug()<< Q_FUNC_INFO << "connecting frameswapped to update" << "index " << index;
        connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    }

    sendMarker(OVTK_StimulationLabel_Base + flickeringSequence->sequence[currentFlicker]);
    sendMarker(OVTK_StimulationId_VisualSteadyStateStimulationStart);

    while(index < flicker[0].size())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    ++currentFlicker;
    state = POST_TRIAL;
}

void SsvepGL::wait(int millisecondsToWait)
{

    qDebug()<< Q_FUNC_INFO;

    // from stackoverflow question:
    // http://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        //        qDebug()<<"Flickering"<<QTime::currentTime();
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100);

    }

}

void SsvepGL::initElements()
{

    qDebug()<< Q_FUNC_INFO;
    double dx = 0.2;
    double dy = 0.25;
    int isNullX = 0, isNullY = 0, sx=1;

    if(nr_elements == 1)
    {
        vertices.resize(POINTS_PER_SQUARE);
        vertices[0] = topPoints[0];
        colors.resize(POINTS_PER_SQUARE);
        colors[0] = cWhite;
        for(int i=1; i<POINTS_PER_SQUARE; ++i)
        {
            // init points
            isNullX = i % 2;
            isNullY = (i+1) % 2;
            vertices[i].setX(vertices[i-1].x() + (dx * isNullX *sx));
            vertices[i].setY(vertices[i-1].y() - (dy * isNullY));
            vertices[i].setZ(topPoints[0].z());
            sx--;
            // init colors
            colors[i] = cWhite;
        }

    }
    else
    {
        // init vectors
        int vectorsSize = nr_elements * POINTS_PER_SQUARE;
        vertices.resize(vectorsSize);
        for(int i=0; i<vertices.count(); i+=POINTS_PER_SQUARE)
        {
            vertices[i] = topPoints[i/POINTS_PER_SQUARE];
            sx = 1;
            for(int j=i+1; j<i+POINTS_PER_SQUARE; ++j)
            {
                isNullX = j % 2;
                isNullY = (j+1) % 2;
                vertices[j].setX(vertices[j-1].x() + (dx * isNullX * sx));
                vertices[j].setY(vertices[j-1].y() - (dy * isNullY));
                vertices[j].setZ(topPoints[0].z());
                sx--;
            }
        }
        // init colors
        colors = {cGray, cGray, cGray, cGray};
        colors.resize(vectorsSize);
        for (int i=POINTS_PER_SQUARE; i<colors.count(); i++)
        {
            colors[i] = cWhite;
        }
    }

    // init indices
    vindices.resize(nr_elements*INDICES_PER_SQUARE);
    int k=0; int val = 0;
    for(int i=0; i<(nr_elements*INDICES_PER_SQUARE); i+=INDICES_PER_SQUARE)
    {
        val = 2*k;
        vindices[i] =  val;
        vindices[i+1] = val + 1;
        vindices[i+2] = val + 2;
        vindices[i+3] = vindices[i];
        vindices[i+4] = vindices[i+2];
        vindices[i+5] = val + 3;
        k +=2;
    }

    vaObject.bind();
    colorBuffer.bind();
    colorBuffer.write(0, colors.data(), colors.count() * sizeof(QVector3D)); // number of vertices to avoid * sizeof QVector3D
    vaObject.release();
    colorBuffer.release();

    QOpenGLWindow::update();
}

void SsvepGL::highlightTarget()
{

    qDebug()<< Q_FUNC_INFO;

    if(nr_elements == 1)
    {
        colors = {cGreen, cGreen, cGreen, cGreen};
    }
    else
    {
        int tmp = flickeringSequence->sequence[currentFlicker]-1;
        int squareIndex = tmp+(VERTICES_PER_TRIANGLE*tmp);
        colors[squareIndex] = cGreen;
        colors[squareIndex + 1] = cGreen;
        colors[squareIndex + 2] = cGreen;
        colors[squareIndex + 3] = cGreen;
    }

    vaObject.bind();
    colorBuffer.bind();
    colorBuffer.write(0, colors.data(), colors.count() * sizeof(QVector3D)); // number of vertices to avoid * sizeof QVector3D
    vaObject.release();
    colorBuffer.release();
    //     Schedule a redraw
    QOpenGLWindow::update();
}

void SsvepGL::refreshTarget()
{

    qDebug()<< Q_FUNC_INFO;
    if(nr_elements == 1)
    {
        colors = {cWhite, cWhite, cWhite, cWhite};
    }
    else
    {
        if(flickeringSequence->sequence[currentFlicker] == 1)
        {
            colors[0] = cGray;
            colors[1] = cGray;
            colors[2] = cGray;
            colors[3] = cGray;
        }
        else
        {
            int tmp = flickeringSequence->sequence[currentFlicker]-1;
            int squareIndex = tmp+(VERTICES_PER_TRIANGLE*tmp);
            colors[squareIndex] = cWhite;
            colors[squareIndex + 1] = cWhite;
            colors[squareIndex + 2] = cWhite;
            colors[squareIndex + 3] = cWhite;
        }
    }
    vaObject.bind();
    colorBuffer.bind();
    colorBuffer.write(0, colors.data(), colors.count() * sizeof(QVector3D)); // number of vertices to avoid * sizeof QVector3D
    vaObject.release();
    colorBuffer.release();
    //     Schedule a redraw
    QOpenGLWindow::update();
}

void SsvepGL::update()
{

    qDebug()<< "[update ]Index : "<< index << "current time: " << QTime::currentTime().msec();
    int k;

    if(nr_elements == 1)
    { k = 0;}
    else
    {k = POINTS_PER_SQUARE;}

    for(int i = 0; i<flicker.size() ;++i)
    {
        colors[k]   = QVector3D(flicker[i][index], flicker[i][index], flicker[i][index]);
        colors[k+1] = QVector3D(flicker[i][index], flicker[i][index], flicker[i][index]);
        colors[k+2] = QVector3D(flicker[i][index], flicker[i][index], flicker[i][index]);
        colors[k+3] = QVector3D(flicker[i][index], flicker[i][index], flicker[i][index]);
        k += POINTS_PER_SQUARE;
    }

    vaObject.bind();
    colorBuffer.bind();
    colorBuffer.write(0, colors.data(), colors.count() * sizeof(QVector3D)); // number of vertices to avoid * sizeof QVector3D
    vaObject.release();
    colorBuffer.release();
    ++index;
    // Schedule a redraw
    QOpenGLWindow::update();
}


// Setters
void SsvepGL::setFrequencies(QString freqs)
{
    qDebug()<< Q_FUNC_INFO;

    QStringList freqsList = freqs.split(',');

    if (nr_elements == 1)
    {

        //        frequencies[0] = freqsList[0].toDouble();
        frequencies.append(freqsList[0].toDouble());
    }
    else
    {
        foreach(QString str, freqsList)
        {

            frequencies.append(str.toDouble());
        }
    }

}

void SsvepGL::setFlickeringMode(int mode)
{
    flickering_mode = mode;
}

void SsvepGL::setStimulationDuration(float stimDuration)
{
    stimulationDuration = stimDuration;
}

void SsvepGL::setBreakDuration(int brkDuration)
{
    breakDuration = brkDuration;
    pre_trial_wait = brkDuration;
}

void SsvepGL::setSequence(int sequence)
{
    stimulationSequence = sequence;
}

void SsvepGL::setFeedbackPort(int port)
{
    feedbackPort = port;
}

SsvepGL::~SsvepGL()
{
    makeCurrent();
}
