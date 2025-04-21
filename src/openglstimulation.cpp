#include <QPainter>
//
#include "openglstimulation.h"
#include "layoutfactory.h"
#include "glutils.h"
#include "utils.h"
//
OpenGLStimulation::OpenGLStimulation(SSVEP *paradigm)
{
    // qDebug()<< Q_FUNC_INFO;
    setFrequencies(paradigm->frequencies());
    initFormat();
    m_layout = LayoutFactory::layoutFactory(paradigm->map());
    // set m_flicker size and fill the flickering values (jfpm etc)
    m_flicker.resize(m_frequencies.size());
    initIntensity(paradigm->stimulationDuration(), paradigm->stimulationMode()); // flicker values

    correctortimer = new QElapsedTimer();
    QString loggerFname = QCoreApplication::applicationDirPath() + "/frame_log_win11_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH.mm.ss.zzz") + ".csv";
    logger = new FrameLogger(loggerFname);
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
    // qDebug()<< "[update ] Index : "<< m_index << "current time: " << currt-time_tmp;
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

void OpenGLStimulation::initElements()
{
    initVectors(); // init vectors
    //
    initCenters();  // circles centers points
    initFlickers(); // vertices
    initColors();  // vertices' colors
    initIndices(); // vertices indices
    //
    scheduleRedraw();
}

void OpenGLStimulation::initIntensity(int t_stimDur, QString t_stimMode)
{
    double phase = 0.0;
    for (int i=0; i < m_frequencies.size(); ++i)
    {
        // phase = config::PHASE * ((i+1)%2);
        phase = config::PHASE * (i%2);
        m_flicker[i] = utils::gen_flick(m_frequencies[i],
                                        config::REFRESH_RATE,
                                        t_stimDur,
                                        t_stimMode,
                                        phase);
    }
}

void OpenGLStimulation::initVectors()
{
    if(m_layout->flickerShape().compare("square") == 0)
    {
        // TODO
        initVectorsSquares();
    }

    else if (m_layout->flickerShape().compare("circle") == 0)
    {
        initVectorsCircles();
    }
}

void OpenGLStimulation::initCenters()
{
    if(m_layout->shape().compare("grid", Qt::CaseInsensitive) == 0)
    {
        // TODO
        initCentersGrid();
    }
    else if(m_layout->shape().compare("circular", Qt::CaseInsensitive) == 0)
    {
        // TODO
        initCentersCircular();
    }

    /*
    // int n_elements = m_ssvep->nrElements();
    int n_elements   = m_frequencies.length();
    QSize screenSize = utils::getScreenSize();
    float radiusx = glUtils::STIM_RADIUS / (screenSize.width() * glUtils::PIXEL_CM);
    float radiusy = glUtils::STIM_RADIUS / (screenSize.height() * glUtils::PIXEL_CM);
    int start = 0;

    m_centerPoints.resize(n_elements);

    for (int j = 0; j<=n_elements; ++j)
    {

        m_centerPoints[j].setX(refPoints::grid_centers[j].x());
        m_centerPoints[j].setY(refPoints::grid_centers[j].y());
        m_centerPoints[j].setZ(refPoints::grid_centers[j].z());
    }
    */
}

void OpenGLStimulation::initFlickers()
{
    if(m_layout->flickerShape().compare("square") == 0)
    {
        // TODO
        initSquares();
    }

    else if (m_layout->flickerShape().compare("circle") == 0)
    {
        initCircles();
    }
}

void OpenGLStimulation::initColors()
{
    if(m_layout->flickerShape().compare("square") == 0)
    {
        // TODO
        initColorsSquares();
    }

    else if (m_layout->flickerShape().compare("circle") == 0)
    {
        initColorsCircles();
    }
}

void OpenGLStimulation::initIndices()
{
    if(m_layout->flickerShape().compare("square") == 0)
    {
        // TODO
        initIndicesSquares();
    }

    else if (m_layout->flickerShape().compare("circle") == 0)
    {
        initIndicesCircles();
    }
}

void OpenGLStimulation::initVectorsSquares()
{
    // TODO
    m_elements = m_frequencies.length();
    int vectorsSize = m_elements * glUtils::POINTS_PER_SQUARE;
    // int vectorsSize = glUtils::VERTICES_PER_TRIANGLE * (m_ssvep->nrElements()) * glUtils::TRIANGLES_PER_SQUARE;
    vectorsSize += m_elements;
    m_vertices.resize(vectorsSize);
    m_vertexPerCircle = glUtils::POINTS_PER_SQUARE;
    m_vindices.resize(m_elements * glUtils::INDICES_PER_SQUARE);
    m_centerindices.resize(m_elements);
}

void OpenGLStimulation::initVectorsCircles()
{
    int vectorsSize = 0;
    int colorsVectorsSize = 0;
    m_elements = m_frequencies.length();
    m_vertexPerCircle = glUtils::SIDES_PER_CIRCLE + 2;

    vectorsSize = (m_frequencies.length() * m_vertexPerCircle) + m_frequencies.length();
    colorsVectorsSize = (m_elements * m_vertexPerCircle) + m_elements;

    m_vertices.resize(vectorsSize);
    m_colors.resize(colorsVectorsSize);
    m_vindices.resize( 3 * (m_vertexPerCircle * m_elements - (m_elements*2)));
    m_centerindices.resize(m_elements);
}

void OpenGLStimulation::initCentersGrid()
{
    Grid *g = qobject_cast<Grid*>(m_layout);

    QSize screenSize   = utils::getScreenSize();
    int n_elements     = m_frequencies.length();
    int rows = g->rows();
    int cols = g->cols();
    int elementsPerRow = n_elements / rows;
    int elementsPerCol = n_elements / cols;
    int k = 0;
    float xCoord, yCoord;
    QPointF openglCoord;

    int vSpace = (g->verticalSpace() == 0) ? screenSize.height() / (elementsPerCol + 1) : g->verticalSpace();
    int hSpace = (g->horizontalSpace() == 0) ? screenSize.width() / (elementsPerRow + 1) : g->horizontalSpace();

    m_centerPoints.resize(n_elements);

    for(int i=0; i< rows; ++i)
    {
        for(int j=0; j< cols; ++j)
        {
            xCoord = hSpace + (hSpace * j);
            yCoord = vSpace + (vSpace * i);

            openglCoord = openGLCoordinates(xCoord, yCoord, screenSize);

            m_centerPoints[k].setX(openglCoord.x());
            m_centerPoints[k].setY(openglCoord.y());
            m_centerPoints[k].setZ(1.0f);
            ++k;
        }
    }
}

void OpenGLStimulation::initCentersCircular()
{
    // TODO
    Circular *c = qobject_cast<Circular*>(m_layout);

    int n_elements   = m_frequencies.length();
    QSize screenSize = utils::getScreenSize();
    float radiusx = c->radius() / screenSize.width();
    float radiusy = c->radius() / screenSize.height();
    int start  = 0;
    qreal angle= 0;

    m_centerPoints.resize(n_elements);
    // m_centerPoints[0] = QVector3D(0.0f, 0.0f, 1.0f);

    for(int i=start; i<n_elements; ++i)
    {
        // angle = (i * 2 * M_PI) / (n_elements - 1);
        angle = (i * 2 * M_PI) / n_elements;
        m_centerPoints[i].setX(radiusx * cos(angle));
        m_centerPoints[i].setY(radiusy * sin(angle));
        m_centerPoints[i].setZ(1.0f);
    }
}

void OpenGLStimulation::initSquares()
{
    // TODO
    float pixelSize = m_layout->flickerDimension(); // length of a stimulus vertex in pixels
    int n_elements  = m_frequencies.length();
    QSize screenSize = utils::getScreenSize();
    double dx = pixelSize / screenSize.width();
    double dy = pixelSize / screenSize.height();
    int isNullX = 0, isNullY = 0, sx=1;
    int offset;

    int vectorsSize = n_elements * glUtils::POINTS_PER_SQUARE;
    // int vectorsSize = glUtils::VERTICES_PER_TRIANGLE * (m_ssvep->nrElements()) * glUtils::TRIANGLES_PER_SQUARE;
    vectorsSize += n_elements;
    m_vertices.resize(vectorsSize);

    int k = 0; // centers index counter

    /*
    if( m_ssvep->controlMode() == control_mode::SYNC)
    {
        offset = glUtils::POINTS_PER_SQUARE;
    }
    else
    {
        offset = 0;
    }

    */

    // offset = glUtils::POINTS_PER_SQUARE;
    offset = 0;

    for(int i=0;i<m_vertices.count() - n_elements; i+=glUtils::POINTS_PER_SQUARE)
    {
        m_vertices[i] = m_centerPoints[(i+offset)/glUtils::POINTS_PER_SQUARE];
        sx = 1;

        for(int j=i+1; j<i+glUtils::POINTS_PER_SQUARE; ++j)
        {
            isNullX = j % 2;
            isNullY = (j+1) % 2;
            m_vertices[j].setX(m_vertices[j-1].x() + (dx * isNullX * sx));
            m_vertices[j].setY(m_vertices[j-1].y() - (dy * isNullY));
            m_vertices[j].setZ(1.0f);
            sx--;

            // calculate center point
            if (j==(i+2))
            {
                m_centerPoints[k].setX((m_vertices[j].x() + m_vertices[i].x()) / 2);
                m_centerPoints[k].setY((m_vertices[j].y() + m_vertices[i].y()) / 2);
                m_centerPoints[k].setZ(1.0f);
                ++k;
            }
        }
    }

    k = m_vertices.count() - n_elements;

    int i=0;
    for (int ind=k; ind<m_vertices.count(); ++ind)
    {
        m_vertices[ind] = m_centerPoints[i];
        ++i;
    }
}

void OpenGLStimulation::initIndicesCircles()
{
    // init indices

    int circleIndices = 3*glUtils::SIDES_PER_CIRCLE;
    int k = 0;

    for (int i=0; i<m_vindices.count(); i+=3)
    {
        if((i%3)==0)
        {
            m_vindices[i] = m_vertexPerCircle * (i / circleIndices);
        }

        if( (i % circleIndices) == 0)
        {
            k = m_vindices[i];
        }

        m_vindices[i+1] = k+1;
        m_vindices[i+2] = k+2;
        k++;
    }

    // int centerStart = m_vertices.count() - m_ssvep->nrElements();
    // int centerStart = m_vertices.count() - circleCount;
    int centerStart = m_vertices.count() - m_elements;
    for (int i=0; i<m_centerindices.count(); ++i)
    {
        m_centerindices[i] = centerStart + i;
    }
}

void OpenGLStimulation::initIndicesSquares()
{
    // init indices
    // int n_elements = m_frequencies.length();

    m_vindices.resize(m_elements * glUtils::INDICES_PER_SQUARE);
    m_centerindices.resize(m_elements);

    int k=0; int val = 0;
    for(int i=0; i<(m_elements*glUtils::INDICES_PER_SQUARE); i+=glUtils::INDICES_PER_SQUARE)
    {
        val = 2*k;
        m_vindices[i] =  val;
        m_vindices[i+1] = val + 1;
        m_vindices[i+2] = val + 2;
        m_vindices[i+3] = m_vindices[i];
        m_vindices[i+4] = m_vindices[i+2];
        m_vindices[i+5] = val + 3;
        k +=2;
    }

    int centerStart = m_vertices.count() - m_elements;

    for (int i=0;i<m_centerindices.count();++i)
    {
        m_centerindices[i] = centerStart + i;
    }
}

void OpenGLStimulation::initCircles()
{
    float twicePi = 2.0f * M_PI;
    int start = 0;
    int stop  = m_vertexPerCircle;
    int k = 0;
    // int elements = m_frequencies.length();

    QSize screenSize = utils::getScreenSize();

    float radiusx = m_layout->flickerDimension() / screenSize.width();
    float radiusy = m_layout->flickerDimension() / screenSize.height();

    // float radiusx = glUtils::RADIUS_CM / (screenSize.width() * glUtils::PIXEL_CM);
    // float radiusy = glUtils::RADIUS_CM / (screenSize.height() * glUtils::PIXEL_CM);
    float x, y, z, xx, yy;

    /*
    if(m_ssvep->controlMode() == control_mode::SYNC)
    {
        start = 0;
    }
    else
    {
        --elements;
    }
    */

    for (int j = start; j<m_elements; ++j)
    {
        x = m_centerPoints[j].x();
        y = m_centerPoints[j].y();
        z = m_centerPoints[j].z();

        setVertex(k, x, y, z);

        // circles vertices
        for ( int i = k+1; i < stop; i++ )
        {
            xx = (x + (radiusx * cos(i * twicePi / glUtils::SIDES_PER_CIRCLE)));
            yy = (y + (radiusy * sin(i * twicePi /glUtils::SIDES_PER_CIRCLE)));
            setVertex(i, xx, yy, z);
        }
        k = stop;
        stop += m_vertexPerCircle;
    }

    k = m_vertices.count() - m_elements;

    // center points vertices
    int i = start;
    for (int ind=k; ind<m_vertices.count();++ind)
    {
        setVertex(ind, m_centerPoints[i].x(), m_centerPoints[i].y(), m_centerPoints[i].z());
        ++i;
    }
}

void OpenGLStimulation::initColorsCircles()
{
    for (int i=0; i<m_colors.count(); i++)
    {
        if (i < m_colors.count() - m_elements)
        {
            m_colors[i] = glColors::white;
        }
        else
        {
            m_colors[i] = glColors::red; // center points //TODO convert colors to glcolor
        }
    }

    /*
    if(m_ssvep->controlMode() == control_mode::SYNC)
    {
        for (int i=0; i<m_colors.count(); i++)
        {
            if (i < m_colors.count() - m_ssvep->nrElements())
            {
                m_colors[i] = glColors::white;
            }
            else
            {
                m_colors[i] = glColors::red;
            }
        }
    }
    else
    {
        // init colors
        // center idle stim
        for (int i=0; i<m_vertexPerCircle; ++i)
        {
            m_colors[i] = glColors::gray;
        }

        for (int i=m_vertexPerCircle; i<m_colors.count(); i++)
        {
            if (i < m_colors.count() - m_ssvep->nrElements())
            {
                m_colors[i] = glColors::white;
            }
            else
            {
                m_colors[i] = glColors::red;
            }
        }
    }

    */
}

void OpenGLStimulation::initColorsSquares()
{
    //int n_elements = m_frequencies.length();
    int vectorsSize = m_elements * glUtils::POINTS_PER_SQUARE;

    vectorsSize += m_elements; // for centers
    m_colors.resize(vectorsSize);

    for (int i=0; i<m_colors.count(); i++)
    {
        if (i < m_colors.count() - m_elements)
        {
            m_colors[i] = glColors::white;
        }
        else
        {
            m_colors[i] = glColors::red;
        }
    }
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
    if(m_layout->flickerShape().compare("square") == 0)
    {
        // TODO
        highlightSquares(feedbackColor, feedbackIndex);
    }

    else if (m_layout->flickerShape().compare("circle") == 0)
    {
        highlightCircles(feedbackColor, feedbackIndex);
    }
}

void OpenGLStimulation::refresh(int feedbackIndex)
{
    if(m_layout->flickerShape().compare("square") == 0)
    {
        // TODO
        refreshSquares(feedbackIndex);
    }

    else if (m_layout->flickerShape().compare("circle") == 0)
    {
        refreshCircles(feedbackIndex);
    }
}

void OpenGLStimulation::highlightCircles(QVector3D feedbackColor, int feedbackIndex)
{
    int circleIndex = m_vertexPerCircle*feedbackIndex;

    for(int i=circleIndex; i<circleIndex+m_vertexPerCircle; ++i)
    {
        m_colors[i] = feedbackColor;
    }

    scheduleRedraw();
}

void OpenGLStimulation::highlightSquares(QVector3D feedbackColor, int feedbackIndex)
{
    // TODO
    int squareIndex = feedbackIndex + (glUtils::VERTICES_PER_TRIANGLE* feedbackIndex);

    m_colors[squareIndex]     = feedbackColor;
    m_colors[squareIndex + 1] = feedbackColor;
    m_colors[squareIndex + 2] = feedbackColor;
    m_colors[squareIndex + 3] = feedbackColor;

    scheduleRedraw();
}

void OpenGLStimulation::refreshCircles(int feedbackIndex)
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

void OpenGLStimulation::refreshSquares(int feedbackIndex)
{
    // TODO
    // int tmp = m_flickeringSequence->sequence[m_currentFlicker]-1;
    int squareIndex = feedbackIndex + (glUtils::VERTICES_PER_TRIANGLE * feedbackIndex);

    m_colors[squareIndex] = glColors::white;
    m_colors[squareIndex + 1] = glColors::white;
    m_colors[squareIndex + 2] = glColors::white;
    m_colors[squareIndex + 3] = glColors::white;
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

// Getters and Setters
void OpenGLStimulation::setVertex(int t_index, float x, float y, float z)
{
    m_vertices[t_index].setX(x);
    m_vertices[t_index].setY(y);
    m_vertices[t_index].setZ(z);
}

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

