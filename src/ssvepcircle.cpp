#include <QWindow>
#include <QDebug>
#include <QTime>
#include <QtMath>
#include <Qapplication>
#include <QDateTime>
#include <QDir>
#include <QPainter>
#include <QFont>
#include <array>
//
#include "ssvepcircle.h"
#include "ovtk_stimulations.h"
#include "utils.h"
#include "glutils.h"
//
SsvepCircle::SsvepCircle(SSVEP *paradigm, int t_port)
{
    m_ssvep = paradigm;
    setFrequencies(m_ssvep->frequencies());
    setFeedbackPort(t_port);

    m_preTrialWait = 3;

    // set vertices, vertices indices & colors
    initElements();

    m_preTrialTimer = new QTimer(this);
    m_preTrialTimer->setTimerType(Qt::PreciseTimer);
    m_preTrialTimer->setInterval(m_ssvep->breakDuration() / 2);
    m_preTrialTimer->setSingleShot(true);

    connect(m_preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );

    m_feedbackSocket = new QUdpSocket(this);
    m_feedbackSocket->bind(QHostAddress::LocalHost, m_feedbackPort);

    connect(m_feedbackSocket, SIGNAL(readyRead()), this, SLOT(receiveFeedback()));
    //
    initLogger();
    //
    m_state = trial_state::PRE_TRIAL;
}

void SsvepCircle::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();

    m_index = 0;
    // Set global information
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE); //
    glEnable(GL_FRAMEBUFFER_SRGB);

    m_flicker.resize(m_frequencies.size());
    qDebug()<< Q_FUNC_INFO << m_frequencies;

    double phase = 0.0;
    for (int i=0; i < m_frequencies.size(); ++i)
    {
        // phase = config::PHASE * ((i+1)%2);
        phase = config::PHASE * (i%2);
        m_flicker[i] = utils::gen_flick(m_frequencies[i], config::REFRESH_RATE, m_ssvep->stimulationDuration(), m_ssvep->stimulationMode(), phase);
    }

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

    initExternalSocket();
}

void SsvepCircle::resizeGL(int w, int h)
{
    //TODO
    //    (void)w;
    //    (void)h;
    initElements();
}

void SsvepCircle::paintGL()
{
    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    int centerVertices = m_ssvep->nrElements();
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

    if (m_ssvep->experimentMode() == operation_mode::FREE_MODE)
    {
        renderText();
    }

    // a hacky way to render text with feedback
    if(m_state == trial_state::POST_TRIAL && m_showExternalFeedback && m_receivedFeedback)
    {
        renderFeedBackText();
    }
}

void SsvepCircle::startTrial()
{
    if (m_state == trial_state::PRE_TRIAL)
    {
        preTrial();
    }
    if (m_state == trial_state::STIMULUS)
    {
        Flickering();
    }
    if (m_state == trial_state::POST_TRIAL)
    {
        postTrial();
    }
}

void SsvepCircle::preTrial()
{
    m_receivedFeedback = false;

    if(m_trials == 0)
    {
        sendMarker(OVTK_StimulationId_ExperimentStart);
    }

    if(m_firstRun)
    {
        m_flickeringSequence = new RandomFlashSequence(m_ssvep->nrElements(), m_ssvep->nrSequences() / m_ssvep->nrElements());
        m_firstRun = false;
    }

    if (m_preTrialCount == 1)
    {
        sendMarker(OVTK_StimulationId_TrialStart);

        if (m_ssvep->experimentMode() == operation_mode::CALIBRATION ||
                m_ssvep->experimentMode() == operation_mode::COPY_MODE)
        {
            highlightTarget();
        }
    }

    else if(m_preTrialCount == 2)
    {
        if (m_ssvep->experimentMode() == operation_mode::CALIBRATION ||
                m_ssvep->experimentMode() == operation_mode::COPY_MODE)
        {
            // refresh only for idle
            if (m_ssvep->controlMode() == control_mode::ASYNC && m_flickeringSequence->sequence[m_currentFlicker] == 1)
            {
                refreshTarget();
            }
        }
    }

    m_preTrialTimer->start();
    m_preTrialCount++;

    if (m_preTrialCount > m_preTrialWait)
    {
        scheduleRedraw();
        m_preTrialTimer->stop();
        m_preTrialCount = 1;
        m_state = trial_state::STIMULUS;
    }
}

void SsvepCircle::postTrial()
{
    // initElements();
    // qDebug()<< Q_FUNC_INFO;
    disconnect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    refreshCircles(); // initElements();

    m_index = 0;
    //  m_state = trial_state::PRE_TRIAL;

    // feedback
    if(m_ssvep->experimentMode() == operation_mode::COPY_MODE ||
            m_ssvep->experimentMode() == operation_mode::FREE_MODE)
    {
        feedback();  // wait for feedback

        if (m_presentFeedback)
        {
            // feedback for 0.5 sec & refresh
            // utils::wait(300);
            utils::wait(500);
            refresh(m_sessionFeedback[m_currentFlicker].digitValue()-1);
        }
    }

    // calibration mode
    else
    {
        refreshTarget();
    }

    m_state = trial_state::PRE_TRIAL;
    // refreshCircles();

    externalCommunication();

    postTrialEnd();
}

void SsvepCircle::postTrialEnd()
{
    ++m_currentFlicker;
    ++m_trials;

    if (m_currentFlicker < m_flickeringSequence->sequence.size() &&
            m_flickeringSequence->sequence.length() != 1 &&
            (m_ssvep->experimentMode() == operation_mode::COPY_MODE ||
             m_ssvep->experimentMode() == operation_mode::CALIBRATION ||
             m_ssvep->experimentMode() == operation_mode::FREE_MODE))
    {
        startTrial();
    }
    else if (m_flickeringSequence->sequence.length() <= 1)
    {
        m_currentFlicker = 0;
        emit(slotTerminated());
        return;
    }
    else
    {
        m_correct = (m_correct / m_flickeringSequence->sequence.size()) * 100;
        qDebug()<< "Accuracy in SSVEP session: " << m_correct;
        qDebug()<< "Experiment End, closing SSVEP stimulation";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        utils::wait(2000);
    }
}

void SsvepCircle::Flickering()
{
    if(m_index == 0)
    {
        connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    }

    while(m_index <= m_flicker[0].size())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (m_receivedFeedback)
        {

            break;
        }
    }

    /*
    if (m_index > m_flicker[0].size())
    {
        initElements();
    }
    */

    sendMarker(OVTK_StimulationId_TrialStop);
    m_state = trial_state::POST_TRIAL;
}

void SsvepCircle::feedback()
{
    // receiveFeedback
    if (!m_receivedFeedback)
    {
        m_feedbackSocket->waitForReadyRead();
    }

    if(m_presentFeedback)
    {
        if(m_ssvep->experimentMode() == operation_mode::COPY_MODE)
        {
            if(m_sessionFeedback[m_currentFlicker].digitValue() == m_flickeringSequence->sequence[m_currentFlicker])
            {
                highlightFeedback(glColors::green, m_flickeringSequence->sequence[m_currentFlicker]-1);
                ++m_correct;
            }
            else
            {
                highlightFeedback(glColors::blue, m_sessionFeedback[m_currentFlicker].digitValue()-1);
            }
        }
        else if(m_ssvep->experimentMode() == operation_mode::FREE_MODE)
        {
            highlightFeedback(glColors::red, m_sessionFeedback[m_currentFlicker].digitValue()-1);
        }
    }
}

void SsvepCircle::receiveFeedback()
{
    // qDebug()<< Q_FUNC_INFO << "Feedback received";
    // wait for OV python script to write in UDP feedback socket
    // wait(500);
    QHostAddress sender;
    quint16 senderPort;
    QByteArray *buffer = new QByteArray();

    buffer->resize(m_feedbackSocket->pendingDatagramSize());

    while(m_feedbackSocket->hasPendingDatagrams())
    {
        m_feedbackSocket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
    }

    log->write(buffer->data());

    //  if(!strcmp(buffer->data(),"-1"))
    //  {
    if (m_flickeringSequence->sequence.length() == 1) // Hybrid stimulation mode
    {
        m_sessionFeedback = buffer->data();
    }
    else
    {
        m_sessionFeedback += buffer->data();
    }
    // }

    m_receivedFeedback = true;
}

void SsvepCircle::initElements()
{
    // init vectors
    // int m_vertexPerCircle = glUtils::SIDES_PER_CIRCLE + 2;
    m_vertexPerCircle = glUtils::SIDES_PER_CIRCLE + 2;
    int vectorsSize = (m_ssvep->nrElements() * m_vertexPerCircle) + m_ssvep->nrElements();
    m_vertices.resize(vectorsSize);
    //
    initCenters(); // circles centers points
    initCircles(); // vertices
    initColors();  // vertices' colors
    initIndices(); // vertices indices
    //
    scheduleRedraw();
}

void SsvepCircle::initCenters()
{
    int n_elements = m_ssvep->nrElements();
    QSize screenSize = utils::getScreenSize();
    float radiusx = glUtils::STIM_RADIUS / (screenSize.width() * glUtils::PIXEL_CM);
    float radiusy = glUtils::STIM_RADIUS / (screenSize.height() * glUtils::PIXEL_CM);
    int start = 0;

    m_centerPoints.resize(n_elements);

    // circular layout, experimental
    /*
    if(m_ssvep->controlMode() == control_mode::ASYNC)
    {
        m_centerPoints[0] = QVector3D(0.0f, 0.0f, 1.0f);
        --n_elements;
        start = 1;
    }

    for(int i=start; i<=n_elements; ++i)
    {
         m_centerPoints[i].setX(radiusx * cos(i));
         m_centerPoints[i].setY(radiusy * sin(i));
         m_centerPoints[i].setZ(1.0f);
    }
    */

    for (int j = 0; j<=n_elements; ++j)
    {
        // circles center points
        m_centerPoints[j].setX(refPoints::centers[j].x());
        m_centerPoints[j].setY(refPoints::centers[j].y());
        m_centerPoints[j].setZ(refPoints::centers[j].z());
    }
}

void SsvepCircle::initCircles()
{
    float twicePi = 2.0f * M_PI;
    int start = 0;
    int stop = m_vertexPerCircle;
    int k=0;
    int elements = m_ssvep->nrElements();
    qDebug()<< Q_FUNC_INFO << elements;
    QSize screenSize = utils::getScreenSize();
    float radiusx = glUtils::RADIUS_CM / (screenSize.width() * glUtils::PIXEL_CM);
    float radiusy = glUtils::RADIUS_CM / (screenSize.height() * glUtils::PIXEL_CM);
    float x, y, z, xx, yy;

    if(m_ssvep->controlMode() == control_mode::SYNC)
    {
        start = 1;
    }
    else
    {
        --elements;
    }


    qDebug()<< Q_FUNC_INFO <<"after" << elements;
    m_centers.resize(m_ssvep->nrElements());

    for (int j = start; j<=elements; ++j)
    {
        qDebug()<<"j "<<j;
        // circles center points
        // x = refPoints::centers[j].x();
        // y = refPoints::centers[j].y();
        // z = refPoints::centers[j].z();
        x = m_centerPoints[j].x();
        y = m_centerPoints[j].y();
        z = m_centerPoints[j].z();

        setVertex(k, x, y, z);

        // circles vertices
        for ( int i = k+1; i < stop; i++ )
        {
            // xx = (x + (glUtils::RADIUS * cos(i * twicePi / glUtils::SIDES_PER_CIRCLE)));
            // yy = (y + (glUtils::RADIUS * sin(i * twicePi /glUtils::SIDES_PER_CIRCLE)));
            xx = (x + (radiusx * cos(i * twicePi / glUtils::SIDES_PER_CIRCLE)));
            yy = (y + (radiusy * sin(i * twicePi /glUtils::SIDES_PER_CIRCLE)));
            setVertex(i, xx, yy, z);
        }
        k = stop;
        stop += m_vertexPerCircle;
    }

    k = m_vertices.count() - m_ssvep->nrElements();

    // center points vertices
    int i=start;
    for (int ind=k; ind<m_vertices.count();++ind)
    {
        //  setVertex(ind, refPoints::centers[i].x(), refPoints::centers[i].y(), refPoints::centers[i].z());
        setVertex(ind, m_centerPoints[i].x(), m_centerPoints[i].y(), m_centerPoints[i].z());
        ++i;
    }

    // qDebug()<< Q_FUNC_INFO << m_ssvep->nrElements() << m_vertices.count();
}

void SsvepCircle::initColors()
{
    int vectorsSize = (m_ssvep->nrElements() * m_vertexPerCircle) + m_ssvep->nrElements();
    qDebug()<< "vectorsize"<< vectorsSize;
    m_colors.resize(vectorsSize);

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
    //   qDebug()<< Q_FUNC_INFO << m_colors.count(); // << m_colors;
}

void SsvepCircle::initIndices()
{
    // init indices
    int circleCount = m_ssvep->nrElements();
    qDebug()<<"circlecount" << circleCount;
    m_vindices.resize(3*(m_vertexPerCircle*circleCount-(circleCount*2)));
    m_centerindices.resize(m_ssvep->nrElements());

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

    int centerStart = m_vertices.count() - m_ssvep->nrElements();
    for (int i=0; i<m_centerindices.count(); ++i)
    {
        m_centerindices[i] = centerStart + i;
    }

    //  qDebug()<< Q_FUNC_INFO << m_vindices.count() << m_centerindices;
    //  qDebug()<< Q_FUNC_INFO << m_vindices.count() << m_vindices;
}

void SsvepCircle::refreshCircles()
{
    // qDebug()<< Q_FUNC_INFO ;
    initColors();  // vertices' colors
    //
    scheduleRedraw();
}

void SsvepCircle::externalCommunication()
{
    // Send and Recieve feedback to/from Robot if external communication is enabled
    std::string cmd = QString(m_sessionFeedback[m_currentFlicker]).toStdString();

    if(m_ssvep->externalComm() == external_comm::ENABLED)
    {
        qDebug() << "Sending Feedback to Robot";
        if (!m_robotSocket->isOpen())
        {
            qDebug()<< "Not sending Feedback to Robot : Cannot send feedback socket is not open";
        }

        try
        {
            std::string str = cmd;
            const char* p = str.c_str();
            QByteArray byteovStimulation;
            QDataStream streamovs(&byteovStimulation, QIODevice::WriteOnly);
            streamovs.setByteOrder(QDataStream::LittleEndian);
            streamovs.writeRawData(p, str.length());
            m_robotSocket->write(byteovStimulation);
            m_robotSocket->waitForBytesWritten();
        }
        catch(...)
        {
            qDebug() <<"Send Feedback to Robot, Issue With writting Data";
        }

        qDebug() << "Recieve State from Robot";
        m_robotSocket->waitForReadyRead();

        QByteArray robotState = m_robotSocket->readAll();

        quint8 rState = robotState.toUInt();
        qDebug()<< Q_FUNC_INFO << "Robot State recieved " << rState;
        if(rState == robot_state::READY)
        {
            qDebug()<< "Correct State";
            m_state = trial_state::PRE_TRIAL;
        }
    }
}

void SsvepCircle::highlightTarget()
{
    // int m_vertexPerCircle = glUtils::SIDES_PER_CIRCLE + 2;

    if(m_ssvep->nrElements() == 1)
    {
        for (int i=0; i<m_vertexPerCircle; ++i)
        {
            m_colors[i] = glColors::yellow;
        }
    }
    else
    {
        int tmp = m_flickeringSequence->sequence[m_currentFlicker]-1;
        int circleIndex = m_vertexPerCircle*tmp;
        for(int i=circleIndex;i<circleIndex+m_vertexPerCircle; ++i)
        {
            m_colors[i] = glColors::yellow;
        }

        // qDebug()<< Q_FUNC_INFO << tmp << circleIndex;
    }

    scheduleRedraw();
}

void SsvepCircle::refreshTarget()
{
    // Single element
    if(m_ssvep->nrElements() == 1)
    {
        for (int i=0; i<m_vertexPerCircle; ++i)
        {
            m_colors[i] = glColors::white;
        }
    }
    else
    {
        int tmp = m_flickeringSequence->sequence[m_currentFlicker]-1;
        int circleIndex = m_vertexPerCircle*tmp;
        // Sync Mode
        if( m_ssvep->controlMode() == control_mode::SYNC)
        {
            for(int i=circleIndex;i<circleIndex+m_vertexPerCircle; ++i)
            {
                m_colors[i] = glColors::white;
            }
        }
        // ASync Mode
        else
        {
            if(m_flickeringSequence->sequence[m_currentFlicker] == 1)
            {
                for(int i=0; i<m_vertexPerCircle; ++i)
                {
                    m_colors[i] = glColors::gray;
                }
            }
            else
            {
                for(int i=circleIndex;i<circleIndex+m_vertexPerCircle; ++i)
                {
                    m_colors[i] = glColors::white;
                }
            }
        }
    }

    scheduleRedraw();
}

void SsvepCircle::highlightFeedback(QVector3D feedbackColor, int feedbackIndex)
{
    // qDebug()<< Q_FUNC_INFO;
    int circleIndex = m_vertexPerCircle*feedbackIndex;
    for(int i=circleIndex; i<circleIndex+m_vertexPerCircle; ++i)
    {
        m_colors[i] = feedbackColor;
    }

    scheduleRedraw();
}

void SsvepCircle::refresh(int feedbackIndex)
{
    int circleIndex = m_vertexPerCircle*feedbackIndex;
    QVector3D color;

    if(m_ssvep->controlMode() == control_mode::ASYNC)

    {
        if (feedbackIndex == 0)
        {
            color = glColors::gray;
        }
    }
    else
    {
        color = glColors::white;
    }

    for(int i=circleIndex;i<circleIndex+m_vertexPerCircle; ++i)
    {
        m_colors[i] = color;
    }

    scheduleRedraw();
}

void SsvepCircle::initLogger()
{
    QDir logsDir(QCoreApplication::applicationDirPath() + "/logs");
    if(!logsDir.exists())
    {
        logsDir.mkdir(logsDir.path());
    }
    QString fileName;
    if(m_ssvep->experimentMode() == operation_mode::CALIBRATION)
    {

        fileName = logsDir.filePath("ssvep_calib_" +QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss")+".txt");
    }
    else
    {
        fileName =  logsDir.filePath("ssvep_online_" +QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss")+".txt");
    }

    log = new Logger(this, fileName);
}

void SsvepCircle::scheduleRedraw()
{
    m_vaObject.bind();
    m_colorBuffer.bind();
    m_colorBuffer.write(0, m_colors.data(), m_colors.count() * sizeof(QVector3D)); // number of vertices to avoid * sizeof QVector3D
    m_vaObject.release();
    m_colorBuffer.release();

    QOpenGLWindow::update();
}

void SsvepCircle::renderText()
{
    qDebug()<< Q_FUNC_INFO;

    int screenWidth, screenHeight;
    int x, y;
    QPainter painter(this);

    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 20, 20));

    x = 0;
    y = 0;

    QSize screenSize = utils::getScreenSize();
    screenWidth  = screenSize.width();
    screenHeight = screenSize.height();

    // int k[4] = {1,2,4,3};
    // int k[4] = {1,2,3,4}; // for one-line
    std::array<int, 5> k = {1,2,4,3,0};
    if (m_ssvep->controlMode() == control_mode::ASYNC)
    {
        k = {1,2,3,4,5};
    }

    // for (int i=0; i<m_centers.length(); i++)
    for (int i=0; i<m_centerPoints.length(); i++)
    {
        // leftPixels = leftPercent * screenWidth /2;
        // topPixels = topPercent * screenHeight /2;
        // x = int(m_centers[i].x() * (screenWidth / 2));
        // y = int(m_centers[i].y() * (screenHeight/ 2) - 50);
        // y = int(m_centers[i].y() * (screenHeight/ 2)- 75); // for one-line
        // y = int(m_centers[i].y() * (screenHeight/ 2) + 80);

        x = int(m_centerPoints[i].x() * (screenWidth / 2));
        y = int(m_centerPoints[i].y() * (screenHeight/ 2) + 80);

        // x = 0; y = 0;

        qDebug()<< "x: " << x << " y: "<< y;
        // painter.drawText(x, y, width(), height(), Qt::AlignCenter, QString::number(i+1));
        painter.drawText(x, -y, width(), height(), Qt::AlignCenter, QString::number(k[i]));
    }
}

void SsvepCircle::renderFeedBackText()
{
   // qDebug()<< Q_FUNC_INFO;

    QSize screenSize = utils::getScreenSize();
    int screenWidth, screenHeight;
    int x, y;
    //
    int i = m_sessionFeedback[m_currentFlicker].digitValue();
    // int i = m_externalFeedback;
    QString fbk = QString::number(m_externalFeedback);

    QPainter painter(this);
    // painter.setPen(Qt::red);
    painter.setPen(m_externalFeedbackColor);
    painter.setFont(QFont("Arial", 30, 30));

    screenWidth  = screenSize.width();
    screenHeight = screenSize.height();

    x = int(m_centerPoints[i].x() * (screenWidth / 2));
    y = int(m_centerPoints[i].y() * (screenHeight/ 2));

    painter.drawText(x, -y, width(), height(), Qt::AlignCenter, fbk);

    // qDebug()<<Q_FUNC_INFO << m_externalFeedback;

}

bool SsvepCircle::presentFeedback() const
{
    return m_presentFeedback;
}

SSVEP *SsvepCircle::ssvep() const
{
    return m_ssvep;
}

bool SsvepCircle::isCorrect() const
{
    return m_sessionFeedback[m_sessionFeedback.length()-1].digitValue() == m_flickeringSequence->sequence[m_currentFlicker];
}

void SsvepCircle::update()
{
    // qDebug()<< "[update ] Index : "<< m_index << "current time: " << QTime::currentTime().msec();
    if(m_index == 0)
    {
        sendMarker(config::OVTK_StimulationLabel_Base + m_flickeringSequence->sequence[m_currentFlicker]);
        sendMarker(OVTK_StimulationId_VisualSteadyStateStimulationStart);
    }

    int k, offset;

    if(m_ssvep->nrElements() == 1)
    {
        k = 0;
        offset = 1;
    }
    else
    {
        if (m_ssvep->controlMode() == control_mode::SYNC)
        {
            k = 0;
            offset = 1;
        }
        else
        {
            k = m_vertexPerCircle;
            offset = 2;
        }
    }
    for(int i = 0; i<m_flicker.size() ;++i)
    {
        for(int j=k; j<m_vertexPerCircle*(i+offset); j++)
        {
            m_colors[j] = QVector3D(m_flicker[i][m_index], m_flicker[i][m_index], m_flicker[i][m_index]);
        }
        k += m_vertexPerCircle;
    }

    ++m_index;

    scheduleRedraw();
}

// Setters
void SsvepCircle::setSsvep(SSVEP *ssvep)
{
    m_ssvep = ssvep;
}

void SsvepCircle::setPresentFeedback(bool presentFeedback)
{
    m_presentFeedback = presentFeedback;
}

void SsvepCircle::setVertex(int t_index, float x, float y, float z)
{
    m_vertices[t_index].setX(x);
    m_vertices[t_index].setY(y);
    m_vertices[t_index].setZ(z);
}

bool SsvepCircle::showExternalFeedback() const
{
    return m_showExternalFeedback;
}

void SsvepCircle::setShowExternalFeedback(bool newShowExternalFeedback)
{
    m_showExternalFeedback = newShowExternalFeedback;
}

int SsvepCircle::externalFeedback() const
{
    return m_externalFeedback;
}

void SsvepCircle::setExternalFeedback(int newExternalFeedback, QColor color)
{
    m_externalFeedback = newExternalFeedback;
    m_externalFeedbackColor = color;
}

void SsvepCircle::setFrequencies(QString freqs)
{
    QStringList freqsList = freqs.split(',');

    if (m_ssvep->nrElements() == 1)
    {
        m_frequencies.append(freqsList[0].toDouble());
    }
    else
    {
        foreach(QString str, freqsList)
        {
            m_frequencies.append(str.toDouble());
        }
    }
}

void SsvepCircle::setFeedbackPort(int t_port)
{
    m_feedbackPort = t_port;
}

void SsvepCircle::initExternalSocket()
{
    if(m_ssvep->externalComm() == external_comm::ENABLED)
    {
        qDebug()<< "External Comm is enabled;";
        m_robotSocket = new QTcpSocket();
        m_robotSocket->connectToHost(QHostAddress(m_ssvep->externalAddress()), m_robotPort);

        if(m_robotSocket->waitForConnected())
        {
            qDebug() << "Robot Connection : State Connected";
        }
        else
        {
            qDebug() << "Robot Connection : State Not Connected";
        }
    }
}

SsvepCircle::~SsvepCircle()
{
    makeCurrent();
}
