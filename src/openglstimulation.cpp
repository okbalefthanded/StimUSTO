#include <QPainter>
//
#include "openglstimulation.h"
#include "glutils.h"
#include "utils.h"
//
OpenGLStimulation::OpenGLStimulation(SSVEP *paradigm)
{
    setFrequencies(paradigm->frequencies());
    initFormat();

    correctortimer = new QElapsedTimer();
    QString loggerFname = QCoreApplication::applicationDirPath() + "/frame_log_win11_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH.mm.ss.zzz") + ".csv";
    logger = new FrameLogger(loggerFname);

    // set m_flicker size and fill the flickering values (jfpm etc)
    m_flicker.resize(m_frequencies.size());

    // initElements ?
    // initElements();
}
// OpenGL window interface
void OpenGLStimulation::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();

    m_index = 0;
    // Set global information
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE); //
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Application-specific initialization
    {
        // Create shaders (Do not release until VAO is created)
        m_programShader = new QOpenGLShaderProgram();
        m_programShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/src/shaders/sh_v.vert");
        m_programShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/src/shaders/sh_f.frag");
        m_programShader->link();
        m_programShader->bind();

        // Create buffer (Do not release until VAO is created)
        m_vertexBuffer.create();
        m_vertexBuffer.bind();
        m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_vertexBuffer.allocate(m_vertices.data(), m_vertices.count() * sizeof(QVector3D)); //

        m_colorBuffer.create();
        m_colorBuffer.bind();
        m_colorBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
        m_colorBuffer.allocate(m_colors.data(), m_colors.count() * sizeof(QVector3D));

        // Create Vertex Array Object
        m_vaObject.create();

        m_vaObject.bind();
        m_vertexBuffer.bind();
        m_programShader->enableAttributeArray(0);
        m_programShader->setAttributeBuffer(0, GL_FLOAT, 0, glUtils::TUPLESIZE, 0);
        m_colorBuffer.bind();
        m_programShader->enableAttributeArray(1);
        m_programShader->setAttributeBuffer(1, GL_FLOAT, 0, glUtils::TUPLESIZE, 0);
        m_vaObject.release();

        // Release (unbind) all
        m_vaObject.release();
        m_vertexBuffer.release();
        m_colorBuffer.release();
        m_programShader->release();
    }
}

void OpenGLStimulation::resizeGL(int w, int h)
{
    //TODO
    //    (void)w;
    //    (void)h;
    initElements();
}

void OpenGLStimulation::paintGL()
{
    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    int centerVertices = m_frequencies.length(); // m_ssvep->nrElements();
    //qDebug()<< Q_FUNC_INFO << centerVertices;

    // Render using our shader
    m_programShader->bind();
    {
        m_vaObject.bind();
        glDrawElements(GL_TRIANGLES, m_vindices.count(), GL_UNSIGNED_INT, m_vindices.data());
        // draw rectangles center points in red
        glDrawElements(GL_POINTS, centerVertices, GL_UNSIGNED_INT, m_centerindices.data());
        m_vaObject.release();
    }
    m_programShader->release();

    /* TODO FIXME
    if (m_ssvep->experimentMode() == operation_mode::FREE_MODE)
    {
        renderText();
    }

    // a hacky way to render text with feedback
    if(m_state == trial_state::POST_TRIAL && m_showExternalFeedback && m_receivedFeedback)
    {
        renderFeedBackText();
    }
    */
}

// Graphics manipulations
void OpenGLStimulation::update()
{
    logger->logFrame();
    double currt =  QTime::currentTime().msec();
    qDebug()<< "[update ] Index : "<< m_index << "current time: " << currt-time_tmp;
    time_tmp = currt;

    if(m_index == 0)
    {
        correctortimer->start(); // hacky solution
        // sendMarker(config::OVTK_StimulationLabel_Base + m_flickeringSequence->sequence[m_currentFlicker]);
        // sendMarker(OVTK_StimulationId_VisualSteadyStateStimulationStart);
    }

    int k, offset;
    k = 0;
    offset = 1;

    for(int i = 0; i<m_flicker.size() ;++i)
    {
        for(int j=k; j<m_vertexPerCircle*(i+offset); j++)
        {
            m_colors[j] = QVector3D(m_flicker[i][m_index], m_flicker[i][m_index], m_flicker[i][m_index]); // white stim
            // m_colors[j] = QVector3D(m_flicker[i][m_index], 0.0f, 0.0f); // red stim
            // m_colors[j] = QVector3D(0.0f, m_flicker[i][m_index], 0.0f); // green stim
        }
        k += m_vertexPerCircle;
    }

    ++m_index;
    scheduleRedraw();
    logger->logFrame();
}



void OpenGLStimulation::refreshFlickers()
{
    // qDebug()<< Q_FUNC_INFO ;
    initColors();  // vertices' colors
    //
    scheduleRedraw();
}

void OpenGLStimulation::highlightFeedback(QVector3D feedbackColor, int feedbackIndex)
{
    int circleIndex = m_vertexPerCircle*feedbackIndex;

    for(int i=circleIndex; i<circleIndex+m_vertexPerCircle; ++i)
    {
        m_colors[i] = feedbackColor;
    }

    scheduleRedraw();
}

void OpenGLStimulation::refresh(int feedbackIndex)
{
    int circleIndex = m_vertexPerCircle*feedbackIndex;
    QVector3D color;
    color = glColors::white;

    for(int i=circleIndex;i<circleIndex+m_vertexPerCircle; ++i)
    {
        m_colors[i] = color;
    }

    scheduleRedraw();
}

void OpenGLStimulation::scheduleRedraw()
{
    m_vaObject.bind();
    m_colorBuffer.bind();
    m_colorBuffer.write(0, m_colors.data(), m_colors.count() * sizeof(QVector3D)); // number of vertices to avoid * sizeof QVector3D
    m_vaObject.release();
    m_colorBuffer.release();

    QOpenGLWindow::update();
}

void OpenGLStimulation::renderFeedBackText(int index, QString text)
{
    // qDebug()<< Q_FUNC_INFO;

    QSize screenSize = utils::getScreenSize();
    int screenWidth, screenHeight;
    int x, y;
    //

    QPainter painter(this);
    painter.setPen(m_externalFeedbackColor);
    painter.setFont(QFont("Arial", 30, 30));

    screenWidth  = screenSize.width();
    screenHeight = screenSize.height();

    x = int(m_centerPoints[index].x() * (screenWidth / 2));
    y = int(m_centerPoints[index].y() * (screenHeight/ 2));

    painter.drawText(x, -y, width(), height(), Qt::AlignCenter, text);

    // qDebug()<<Q_FUNC_INFO << m_externalFeedback<< "i" << i;
}

// inits
void OpenGLStimulation::initFormat()
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapInterval(1); // vsync on
    format.setSwapBehavior(QSurfaceFormat::DefaultSwapBehavior);
    // format.setSwapBehavior(QSurfaceFormat::TripleBuffer); //
    format.setVersion(3, 0); // ANGLE supports ES 3.0, higher versions raise exceptions
    setFormat(format);
}

void OpenGLStimulation::initIntensity()
{
    double phase = 0.0;
    for (int i=0; i < m_frequencies.size(); ++i)
    {
        // phase = config::PHASE * ((i+1)%2);
        phase = config::PHASE * (i%2);
        m_flicker[i] = utils::gen_flick(m_frequencies[i], config::REFRESH_RATE, m_ssvep->stimulationDuration(), m_ssvep->stimulationMode(), phase);
    }
}

void OpenGLStimulation::initElements()
{
    // init vectors
    // int m_vertexPerCircle = glUtils::SIDES_PER_CIRCLE + 2;
    m_vertexPerCircle = glUtils::SIDES_PER_CIRCLE + 2;
    int vectorsSize = (m_ssvep->nrElements() * m_vertexPerCircle) + m_ssvep->nrElements();
    m_vertices.resize(vectorsSize);
    //
    initIntensity(); // flicker values
    initCenters();  // circles centers points
    initFlickers(); // vertices
    initColors();  // vertices' colors
    initIndices(); // vertices indices
    //
    scheduleRedraw();
}


// Getters and Setters
QVector<QVector<double> > OpenGLStimulation::flicker() const
{
    return m_flicker;
}

void OpenGLStimulation::setFlicker(const QVector<QVector<double> > &newFlicker)
{
    m_flicker = newFlicker;
}

QVector<double> OpenGLStimulation::frequencies() const
{
    return m_frequencies;
}

void OpenGLStimulation::setFrequencies(QString t_freqs)
{
    QStringList freqsList = t_freqs.split(',');
    foreach(QString str, freqsList)
    {
        m_frequencies.append(str.toDouble());
    }
}


// destructors
OpenGLStimulation::~OpenGLStimulation()
{
    makeCurrent();
}

