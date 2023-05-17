//
#include <QWindow>
#include <QMessageBox>
#include <QGenericMatrix>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QHBoxLayout>
#include <QThread>
#include <QTime>
#include <QPropertyAnimation>
#include <QDateTime>
#include <QPixmap>
#include <QDir>
#include <QPainter>
//
#include "speller.h"
//#include "ui_speller.h"
#include "configpanel.h"
#include "ovtk_stimulations.h"
#include "randomflashsequence.h"
#include "utils.h"
//
// Speller::Speller(QWidget *parent) : QWidget(parent)
Speller::Speller(QWidget *parent) : QDialog(parent)
  //ui(new Ui::Speller)
{
    // qDebug()<< Q_FUNC_INFO;
    // ui->setupUi(this);
    setupUi(this);
    this->setProperty("windowTitle", "ERP Speller");
    // this->setStyleSheet("background-color:white");

    createLayout();
    endInit();
    showWindow();

    // this->setWindowFlag(Qt::Popup);
}

Speller::Speller(quint16 t_port)
{
    // qDebug()<< Q_FUNC_INFO;
    setFeedbackPort(t_port);
    endInit();
}

void Speller::startTrial()
{
    // qDebug()<< Q_FUNC_INFO << m_state;

    if (m_state == trial_state::PRE_TRIAL)
    {
        preTrial();
    }

    if (m_state == trial_state::STIMULUS)
    {
        startFlashing();
    }
    else if (m_state == trial_state::POST_STIMULUS)
    {
        pauseFlashing();
    }
}

void Speller::startFlashing(){}

void Speller::pauseFlashing()
{

    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    // qDebug() << Q_FUNC_INFO << QTime::currentTime().msec();

    //   this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation]-1)->
    //                 widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

    this->layout()->itemAt(m_flashingSequence->sequence[m_currentStimulation]-1)->
            widget()->setProperty("pixmap", m_icons[m_flashingSequence->sequence[m_currentStimulation] - 1]);
    /*
    if (m_ERP->stimulationType() != speller_type::MISMATCH)
    {
        m_element = new QLabel();
        m_element->setPixmap(m_icons[m_flashingSequence->sequence[m_currentStimulation] - 1]);
        m_element->setAlignment(Qt::AlignCenter);

        this->layout()->replaceWidget(this->
                                      layout()->
                                      itemAt(m_flashingSequence->sequence[m_currentStimulation]-1)->
                                      widget(),
                                      m_element,
                                      Qt::FindDirectChildrenOnly);

    }
    else {
        for (int i=0; i<9; i++) {
            m_element = new QLabel();
            m_element->setPixmap(m_icons[i]);
            m_element->setAlignment(Qt::AlignCenter);

            this->layout()->replaceWidget(this->
                                          layout()->
                                          itemAt(i)->
                                          widget(),
                                          m_element,
                                          Qt::FindDirectChildrenOnly);
        }
    }
    */

    switchStimulationTimers();
    ++m_currentStimulation;

    trialEnd();
}

void Speller::preTrial()
{
    if(m_trials == 0)
    {
        experimentStart();
        // initLogger();
    }

    startPreTrial();

    m_preTrialTimer->start();
    ++m_preTrialCount;

    endPreTrial();
}

void Speller::feedback()
{
    receiveFeedback();
    m_textRow->setText(m_text);

    //  qDebug()<< Q_FUNC_INFO << m_text[m_text.length()-1];

    // Presenting Feedback
    if (m_presentFeedback)
    {
        if (m_text[m_text.length()-1] != "#")
        {
            int id = m_text[m_text.length()-1].digitValue();
            QPixmap map = m_icons[id-1];

            if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
            {
                // feedback: green correct selection, highlight the target icon
                //           blue incorrect selection, highlight the selected icon
                if(Correct())
                {
                    //            this->layout()->itemAt(m_currentTarget)->
                    //                    widget()->setStyleSheet("QLabel { color : green; font: 40pt }");
                    map.fill(m_correctColor);
                    isCorrect = true;
                    ++m_correct;
                }
                else
                {
                    // this->layout()->itemAt(m_currentTarget)->
                    //         widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");
                    map.fill(m_incorrectColor);
                    isCorrect = false;
                }
                // qDebug()<< Q_FUNC_INFO << "element id: "<< id;
            }

            else if (m_ERP->experimentMode() == operation_mode::FREE_MODE)
            {
                map.fill(m_incorrectColor);
            }

            this->layout()->itemAt(id-1)->
                    widget()->setProperty("pixmap", map);
        }
    }

    postTrial();
}

void Speller::postTrial()
{
    // qDebug()<< Q_FUNC_INFO;

    ++m_trials;
    // m_currentStimulation = 0;
    // m_state = trial_state::PRE_TRIAL;
    // wait
    // utils::wait(1000);
    // utils::wait(500);
    // utils::wait(250); // showing feedback for 0.25 sec
    // refreshTarget();

    // refreshing feedback
    if (m_presentFeedback)
    {
        // utils::wait(1000);
        // utils::wait(500);
        // utils::wait(250); // showing feedback for 0.25 sec
        utils::wait(100);
        if (m_text[m_text.length()-1] != "#")
        {
            if (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                    m_ERP->experimentMode() == operation_mode::FREE_MODE)
            {
                int id = m_text[m_text.length()-1].digitValue();
                // qDebug()<< Q_FUNC_INFO << "element ID" << id;
                this->layout()->itemAt(id-1)->
                        widget()->setProperty("pixmap", m_icons[id-1]);
            }
            else if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
            {
                refreshTarget();
            }
        }
    }

    // Send and Recieve feedback to/from Robot if external communication is enabled
    // externalCommunication();

    if(m_ERP->externalComm() == external_comm::ENABLED)
    {
        m_externComm->communicate(QString(m_text[m_text.length()-1]));
    }
    // pause for 1 sec / 0.5 sec
    postTrialEnd();

}

void Speller::receiveFeedback()
{
    // wait for OV python script to write in UDP feedback socket
    m_feedbackSocket->waitForReadyRead();
    QHostAddress sender;
    quint16 senderPort;
    QByteArray *buffer = new QByteArray();

    buffer->resize(m_feedbackSocket->pendingDatagramSize());

    while(m_feedbackSocket->hasPendingDatagrams())
    {
        m_feedbackSocket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
    }

    m_text += QString(buffer->data());
}

bool Speller::isTarget()
{
    int index = m_flashingSequence->sequence[m_currentStimulation] - 1;

    if(m_desiredPhrase[m_currentLetter] == m_presentedLetters[index][0])
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Speller::isTarget(int t_stim)
{
    if (m_desiredPhrase[m_currentLetter] == m_presentedLetters[t_stim-1][0])
        return true;
    else
        return false;
}

bool Speller::isAsync()
{
    return m_ERP->controlMode() == control_mode::ASYNC;
}

bool Speller::Correct()
{
    if (m_text.length() == m_desiredPhrase.length())
    {
        return m_text[m_text.length()-1] == m_desiredPhrase[m_text.length()-1];
    }
    else
    {
        return m_text[m_text.length()-1] == m_desiredPhrase[m_desiredPhrase.length()-1];
    }
}

void Speller::highlightTarget()
{
    /*
     * int idx = 0;

    for (int i=0; i<m_rows; i++)
    {
        for (int j=0; j<m_cols; j++)
        {
            //            idx++;
            //            if (desired_phrase[m_currentLetter] == letters[i][j]){
            //                currentTarget = idx;
            //                break;
            if(m_desiredPhrase[m_currentLetter] == m_presentedLetters[idx][0])
            {
                m_currentTarget = idx + 1;
                break;
            }
            idx++;
        }
    }

    */

    // qDebug()<< Q_FUNC_INFO << "current tg "<< m_currentTarget << "current letter " << m_desiredPhrase[m_currentLetter];
    int currentTarget = getCurrentTarget() - 1;

    // this->layout()->itemAt(m_currentTarget-1)->widget()->setProperty("pixmap", m_highlight);
    this->layout()->itemAt(currentTarget)->widget()->setProperty("pixmap", m_highlight);

}

void Speller::refreshTarget()
{
    //   this->layout()->itemAt(m_currentTarget)->
    //           widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
    // qDebug()<< Q_FUNC_INFO << m_currentTarget;
    //int currentTarget = m_currentTarget;

    this->layout()->itemAt(m_currentTarget-1)->
            widget()->setProperty("pixmap", m_icons[m_currentTarget - 1]);
}

void Speller::sendStimulationInfo()
{
    sendMarker(OVTK_StimulationId_VisualStimulationStart);
    sendMarker(config::OVTK_StimulationLabel_Base + m_flashingSequence->sequence[m_currentStimulation]);

    // send target marker
    if (m_ERP->experimentMode() == operation_mode::CALIBRATION ||
            m_ERP->experimentMode() == operation_mode::COPY_MODE)
    {
        if (isTarget())
        {
            sendMarker(OVTK_StimulationId_Target);
        }
        else
        {
            sendMarker(OVTK_StimulationId_NonTarget);
        }
    }
}

void Speller::switchStimulationTimers()
{
    if(m_state == trial_state::STIMULUS)
    {
        m_stimTimer->start();
        m_isiTimer->stop();
        m_state = trial_state::POST_STIMULUS;
    }
    else if(trial_state::POST_STIMULUS)
    {
        m_stimTimer->stop();
        m_isiTimer->start();
        m_state = trial_state::STIMULUS;
    }
}

void Speller::experimentStart()
{
    sendMarker(OVTK_StimulationId_ExperimentStart);
    // setting a pre-trail duration longer for calibration phase
    // than Copy_mode/Free mode phases
    if (m_ERP->experimentMode() == operation_mode::CALIBRATION)
    {
        m_preTrialTimer->setInterval(1000);
    }
    else
    {
        m_preTrialTimer->setInterval(500);
    }
}

void Speller::startPreTrial()
{
    if (m_preTrialCount == 0)
    {
        sendMarker(OVTK_StimulationId_TrialStart);
        m_flashingSequence = new RandomFlashSequence(getnElements(), m_ERP->nrSequences());

        if (m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            highlightTarget();
            m_text += m_desiredPhrase[m_currentLetter];
            m_textRow->setText(m_text);
        }
        else if(m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {
            highlightTarget();
        }
        else if(m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            utils::wait(500);
        }
    }
    else return;
}

void Speller::endPreTrial()
{
    if (m_preTrialCount > m_preTrialWait || m_ERP->experimentMode() == operation_mode::FREE_MODE)
    {
        //   if(m_ERP->experimentMode() == operation_mode::COPY_MODE ||
        //            m_ERP->experimentMode() == operation_mode::CALIBRATION)
        if( m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {
            refreshTarget();
        }
        m_preTrialTimer->stop();
        m_preTrialCount = 0;
        m_state = trial_state::STIMULUS;
    }

    else return;
}

void Speller::postTrialEnd()
{
    utils::wait(500);// 1000
    //
    m_currentStimulation = 0;
    m_state = trial_state::PRE_TRIAL;
    //
    if (m_currentLetter >= m_desiredPhrase.length() &&
            m_desiredPhrase.length() != 1 &&
            (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
             m_ERP->experimentMode() == operation_mode::CALIBRATION)
            )
    {
        m_correct = (m_correct / m_desiredPhrase.length()) * 100;
        qDebug()<< "Accuracy on ERP session: " << m_correct;
        qDebug()<< "Experiment End, closing speller";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        if(m_ERP->externalComm() == external_comm::ENABLED)
        {
            m_externComm->communicate("00");
        }
        utils::wait(2000);
        // emit(slotTerminated());
        // this->close();
    }
    else if(m_desiredPhrase.length() <= 1)
    {
        m_currentLetter = 0;
        emit(slotTerminated());
        return;
    }
    else
    {
        // this->layout()->update();
        startTrial();
    }

}

void Speller::trialEnd()
{
    if (m_currentStimulation >= m_flashingSequence->sequence.count())
    {
        ++m_currentLetter;
        m_isiTimer->stop();
        m_stimTimer->stop();

        // utils::wait(1000); // time window after last epoch/stim
        utils::wait(500);
        // utils::wait(700); // 700 ms == epoch time windows
        sendMarker(OVTK_StimulationId_TrialStop);
        m_state = trial_state::FEEDBACK;

        if(m_ERP->experimentMode() == operation_mode::COPY_MODE || m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            feedback();
        }
        else if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
        {
            postTrial();
        }
    }
}

int Speller::getnElements()
{
    return m_nrElements;
}

void Speller::fillFeedBackMap(QPixmap *map, QColor t_mapColor, QColor t_textColor, QString text)
{
    // qDebug()<< Q_FUNC_INFO<< map << t_mapColor << t_textColor << text;

    map->fill(t_mapColor);
    QPainter painter(map);
    painter.setPen(t_textColor);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(QPoint(25, 50), text);
}

QString Speller::getDesiredPhrase() const
{
    return m_desiredPhrase;
}

void Speller::initLogger()
{

    // qDebug() << Q_FUNC_INFO;

    QDir logsDir(QCoreApplication::applicationDirPath() + "/logs");
    if(!logsDir.exists())
    {
        logsDir.mkdir(logsDir.path());
    }
    QString fileName;
    if(m_ERP->experimentMode() == operation_mode::CALIBRATION)
    {

        fileName = logsDir.filePath("erp_calib_" +QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss")+".txt");
    }
    else
    {
        fileName =  logsDir.filePath("erp_online_" +QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss")+".txt");
    }

    log = new Logger(this, fileName);
}

void Speller::showWindow()
{
    this->show();

    if(qApp->screens().count() == 2)
    {
        this->windowHandle()->setScreen(qApp->screens().last());
        this->showFullScreen();
    }
    else
    {
        // this->showMaximized();
        this->showFullScreen();
    }

    this->setStyleSheet("background-color : black");
}

void Speller::initTimers()
{
    m_stimTimer     = new QTimer(this);
    m_isiTimer      = new QTimer(this);
    m_preTrialTimer = new QTimer(this);

    m_stimTimer->setTimerType(Qt::PreciseTimer);
    m_stimTimer->setSingleShot(true);
    m_stimTimer->setInterval(100); //default value

    m_isiTimer->setTimerType(Qt::PreciseTimer);
    m_isiTimer->setSingleShot(true);
    m_isiTimer->setInterval(100); //default value

    m_preTrialTimer->setTimerType(Qt::PreciseTimer);
    m_preTrialTimer->setInterval(1000);
    // m_preTrialTimer->setInterval(600);
    // m_preTrialTimer->setInterval(500); //
    // m_preTrialTimer->setInterval(200);
    // m_preTrialTimer->setInterval(300);
    m_preTrialTimer->setSingleShot(true);

    connect( m_stimTimer, SIGNAL(timeout()), this, SLOT(pauseFlashing()) );
    connect( m_isiTimer, SIGNAL(timeout()), this, SLOT(startFlashing()) );
    connect( m_preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );
}

void Speller::initFeedbackSocket()
{
    m_feedbackSocket = new QUdpSocket(this);
    m_feedbackSocket->bind(QHostAddress::LocalHost, m_feedbackPort);
}

void Speller::endInit()
{
    // qDebug()<<Q_FUNC_INFO;

    initTimers();
    initFeedbackSocket();
    m_state = trial_state::PRE_TRIAL;
    m_highlight = QPixmap(":/images/bennabi_face_red_inverted.png");
    QLabel label;
    int label_h = label.height() + 40;
    int label_w = label.width() + 40;
    // m_highlight.scaled(label_w, label_h, Qt::KeepAspectRatio);
    m_highlight.scaled(120, 120, Qt::KeepAspectRatio);
    m_highlight.fill(m_highlightColor);
}

ERP *Speller::erp() const
{
    return m_ERP;
}

void Speller::setERP(ERP *erp)
{
    m_ERP = erp;
    setTimers(m_ERP->stimulationDuration(), m_ERP->breakDuration());
    setDesiredPhrase(m_ERP->desiredPhrase());

    m_textRow->setText(m_desiredPhrase);
    // external comm
    // a temporary hack
    // qDebug()<< "Lets see external comm" <<m_ERP->externalComm();

    if(m_ERP->externalComm() == external_comm::ENABLED)
    {
        /*
        qDebug()<< "External Comm is enabled;";
        m_machineSocket = new QTcpSocket();
        //    m_robotSocket->connectToHost(QHostAddress("10.3.66.5"), m_robotPort);
        // 10.3.66.5 / 10.6.65.128 /10.3.64.92
        // m_robotSocket->connectToHost(QHostAddress("10.3.64.92"), m_robotPort);
        m_machineSocket->connectToHost(QHostAddress(m_ERP->externalAddress()), m_machinePort);

        if(m_machineSocket->waitForConnected())
        {
            qDebug() << "Robot Connection : State Connected";
        }
        else
        {
            qDebug() << "Robot Connection : State Not Connected";
        }
        */
        m_externComm = new ExternComm(m_ERP->externalAddress(), 12347, m_ERP->externalComm());
    }
}

bool Speller::presentFeedback() const
{
    return m_presentFeedback;
}

void Speller::setTimers(int t_stimulation, int t_isi)
{
    m_stimTimer->setTimerType(Qt::PreciseTimer);
    m_stimTimer->setSingleShot(true);
    m_stimTimer->setInterval(t_stimulation);

    m_isiTimer->setTimerType(Qt::PreciseTimer);
    m_isiTimer->setSingleShot(true);
    m_isiTimer->setInterval(t_isi);
}

void Speller::setPresentFeedback(bool t_do)
{
    m_presentFeedback = t_do;
}

void Speller::setFeedbackPort(quint16 t_port)
{
    m_feedbackPort = t_port;
}

void Speller::showFeedback(QString command, bool correct)
{
    qDebug()<< Q_FUNC_INFO<< command;
    int id = 0;
    QPixmap map;
    if (command[0] != '#')
    {
        id = command[0].digitValue();
        QString speed = command.at(1);
        QColor mapColor, textColor;
        // qDebug()<< "speed "<< speed << command;
        // present feedbck : copy mode
        if (m_ERP->experimentMode() == operation_mode::COPY_MODE)
        {

            // feedback: green correct selection, highlight the target icon
            //           blue incorrect selection, highlight the selected icon
            map = m_icons[id-1];
            if( m_text[m_text.length()-1] == m_desiredPhrase[m_desiredPhrase.length() - 1])
            {
                mapColor = Qt::green;
                isCorrect = true;
                ++m_correct;
            }
            else
            {
                mapColor = Qt::blue;
                isCorrect = false;
            }

            if (correct)
            {
                textColor = Qt::red;
            }
            else
            {
                textColor = Qt::black;
            }

            fillFeedBackMap(&map, mapColor, textColor, speed);

            this->layout()->itemAt(id-1)->
                    widget()->setProperty("pixmap", map);

        }
        // present feedback: FREE mode
        else if (m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            map = m_icons[id-1];
            fillFeedBackMap(&map, Qt::blue, Qt::black, speed);

            this->layout()->itemAt(id-1)->
                    widget()->setProperty("pixmap", map);

        }
        //
        utils::wait(500);

        // refreshtarget
        if (m_ERP->experimentMode() == operation_mode::COPY_MODE ||
                m_ERP->experimentMode() == operation_mode::FREE_MODE)
        {
            map = m_icons[id-1];
            this->layout()->itemAt(id-1)->
                    widget()->setProperty("pixmap", map);

        }
    }
}

int Speller::getCurrentTarget()
{
    int idx = 0;
    /*
    for (int i=0; i<m_rows; i++)
    {
        for (int j=0; j<m_cols; j++)
        {
            if(m_desiredPhrase[m_currentLetter] == m_presentedLetters[idx][0])
            {
                m_currentTarget = idx + 1;
                break;
            }
            idx++;
        }
    }
    */

    for (int i=0; i<m_nrElements;i++)
    {
        if(m_desiredPhrase[m_currentLetter] == m_presentedLetters[idx][0])
        {
            m_currentTarget = idx + 1;
            break;
        }
        idx++;
    }

    return m_currentTarget;
}

void Speller::setDesiredPhrase(const QString &t_desiredPhrase)
{
    m_desiredPhrase = t_desiredPhrase;
}

void Speller::createLayout()
{
    //  qDebug()<< Q_FUNC_INFO;
    // speller settings
    m_rows = 3;
    m_cols = 3;
    m_nrElements = m_rows * m_cols;
    //    m_icons = QList<QImage>();
    //    m_icons = QList<QLabel>();
    //    m_icons = QList<QLabel*>();
    m_icons   = QList<QPixmap>();
    m_element = new QLabel();
    m_element->setAlignment(Qt::AlignCenter);
    //    this->matrix_height = 640;
    //    this->matrix_width = 480;
    //    this->setGeometry(0, 0, matrix_width, matrix_height);
    //    this->Mlayout = new MatrixLayout(qMakePair(6,6), 6, 6);
    //    QHBoxLayout *genLayout = new QHBoxLayout();
    QGridLayout *layout = new QGridLayout();

    m_textRow = new QLabel(this);
    m_textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    m_textRow->setAlignment(Qt::AlignLeft);
    m_textRow->setObjectName("Desired Phrase Row");
    // textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // layout->addWidget(m_textRow, 0, 0, 1, 0);

    // layout->setHorizontalSpacing(400);
    // layout->setVerticalSpacing(100);
    layout->setHorizontalSpacing(110); //110 for alignment
    // layout->setVerticalSpacing(5); //40

    m_textRow->setText(m_desiredPhrase);
    m_textRow->hide();
    // qDebug()<< Q_FUNC_INFO<< m_desiredPhrase;

    int k = 1;
    QString stimName;
    QPixmap pic;
    QImage iconImage;
    int label_h, label_w;
    // add speller ellements
    Qt::AlignmentFlag alignment = Qt::AlignCenter;
    for(int i=1; i<m_rows+1; i++)
    {
        for(int j=0; j<m_cols; j++)
        {
            QLabel *element = new QLabel(this);

            label_h = element->height() + 40;
            label_w = element->width() + 40;

            stimName = ":/images/" + QString::number(k) + ".png"; // directions images
            pic = QPixmap(stimName);

            // element->setText(letters[i-1][j]);

            // element->setText(QString::number(k));
            // element->setStyleSheet("font: 40pt; color:gray");

            // stimName ="image: url(:/images/" + QString::number(k) + ".png)";

            element->setPixmap(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));
            element->setAlignment(Qt::AlignCenter);

            // element->setStyleSheet(stimName);

            //            iconImage = QImage(stimName);
            //            iconImage.scaled(label_w, label_h, Qt::KeepAspectRatio);
            m_icons.append(pic.scaled(label_w, label_h, Qt::KeepAspectRatio));

            //            m_icons->append(element);
            //            m_icons.append(element);

            // QuickHack: FIXME
            if (j ==0)
            {
                alignment = Qt::AlignRight;
            }
            else if (j==1)
            {
                alignment = Qt::AlignCenter;
            }
            else {
                alignment= Qt::AlignLeft;
            }

            layout->addWidget(element, i, j, alignment);

            m_presentedLetters.append(QString::number(k));
            k++;
            // presented_letters.append(letters[i-1][j]);
        }
    }

    this->setLayout(layout);
    // this->setStyleSheet("background-color:gray"); //brown
}

void Speller::refreshLayout()
{
    // TODO
}

Speller::~Speller()
{
    // delete ui;
}
