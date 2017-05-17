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
//
#include "flashingspeller.h"
#include "ui_flashingspeller.h"
#include "configpanel.h"
#include "ovtk_stimulations.h"
#include "randomflashsequence.h"

static QChar letters[6][6] = {
    {'A','B','C','D','E','F'},
    {'G','H','I','J','K','L'},
    {'M','N','O','Q','P','R'},
    {'S','T','U','V','W','X'},
    {'Y','Z','0','1','2','3'},
    {'4','5','6','7','8','9'},
};

const quint8 PRE_TRIAL = 0;
const quint8 STIMULUS = 1;
const quint8 POST_STIMULUS = 2;
const quint8 FEEDBACK = 3;
const quint8 POST_TRIAL = 4;

const quint8 FLASHING_SPELLER = 0;
const quint8 FACES_SPELLER = 1;

const quint8 CALIBRATION = 0;
const quint8 COPY_MODE  = 1;
const quint8 FREE_MODE = 2;

const uint64_t OVTK_StimulationLabel_Base = 0x00008100;
/**
 * @brief FlashingSpeller::FlashingSpeller
 * @param parent
 */
FlashingSpeller::FlashingSpeller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlashingSpeller)
{
    ui->setupUi(this);

    this->show();

    this->windowHandle()->setScreen(qApp->screens().last());
    this->showFullScreen();
    this->setStyleSheet("background-color : black");

    create_layout();

    stimTimer = new QTimer(this);
    isiTimer = new QTimer(this);
    preTrialTimer = new QTimer(this);

    stimTimer->setTimerType(Qt::PreciseTimer);
    stimTimer->setSingleShot(true);

    isiTimer->setTimerType(Qt::PreciseTimer);
    isiTimer->setSingleShot(true);

    preTrialTimer->setTimerType(Qt::PreciseTimer);
    preTrialTimer->setInterval(1000);
    preTrialTimer->setSingleShot(true);

    connect( stimTimer, SIGNAL(timeout()), this, SLOT(pauseFlashing()) );
    connect( isiTimer, SIGNAL(timeout()), this, SLOT(startFlashing()) );
    connect( preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );

    feedback_socket = new QUdpSocket(this);
    feedback_socket->bind(QHostAddress::LocalHost, feedback_port);

    state = PRE_TRIAL;

}

void FlashingSpeller::startTrial()
{
    qDebug()<< "[TRIAL START]" << Q_FUNC_INFO;

    if (state == PRE_TRIAL)
    {
        //pre_trial
        pre_trial();
    }

    if (state == STIMULUS)
    {
        startFlashing();

    }
    else if (state == POST_STIMULUS){
        pauseFlashing();

    }

    //    else if(state == FEEDBACK){
    //        //FEEDBACK
    //        qDebug("FEEDBACK");
    //        this->layout()->itemAt(0)->
    //                widget()->setStyleSheet("QLabel { color : red; font: 40pt }");


    //    }
    //    else if(state == POST_TRIAL)
    //    {
    //        //POST TRIAL
    //        qDebug("POST_TRIAL");
    //        this->layout()->itemAt(0)->
    //                widget()->setStyleSheet("QLabel { color : white; font: 40pt }");
    //    }

}

void FlashingSpeller::startFlashing()
{
    //    qDebug()<< Q_FUNC_INFO;

    sendMarker(OVTK_StimulationId_VisualStimulationStart);
    sendMarker(OVTK_StimulationLabel_Base + flashingSequence->sequence[currentStimulation]);

    // send target marker
    if (spelling_mode == CALIBRATION || spelling_mode == COPY_MODE)
    {
        if (isTarget())
        {
            // qDebug()<< desired_phrase[currentLetter];
            sendMarker(OVTK_StimulationId_Target);
        }
        else
        {
            sendMarker(OVTK_StimulationId_NonTarget);
        }
    }

    if(speller_type == FACES_SPELLER)
        this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
            widget()->setStyleSheet("image: url(:/images/bennabi_face.png)");

    else if(speller_type == FLASHING_SPELLER)
        this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
            widget()->setStyleSheet("QLabel { color : white; font: 60pt }");


    stimTimer->start();
    //    qDebug("Stim Timer started");
    isiTimer->stop();
    state = POST_STIMULUS;
}

void FlashingSpeller::pauseFlashing()
{
    //    qDebug()<< Q_FUNC_INFO;
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

    stimTimer->stop();
    isiTimer->start();
    //    qDebug("Isi Timer started");
    currentStimulation++;
    state = STIMULUS;

    if (currentStimulation >= flashingSequence->sequence.count())
    {
        currentLetter++;
        //        qDebug("STOPPED: isi Timer & Stim timer");
        isiTimer->stop();
        stimTimer->stop();

        //        if(currentLetter >= desired_phrase.length())
        //        {
        //            qDebug("Experiment End");

        //        }
        //        else
        //        {
        sendMarker(OVTK_StimulationId_TrialStop);
        state = FEEDBACK;

        if(spelling_mode == COPY_MODE || spelling_mode == FREE_MODE){
            feedback();
        }
        else if(spelling_mode == CALIBRATION)
        {
            post_trial();
        }
    }
}

void FlashingSpeller::pre_trial()
{
    qDebug()<< Q_FUNC_INFO;

    if (pre_trial_count == 0)
    {
        flashingSequence = new RandomFlashSequence(nr_elements, nr_sequence);
        sendMarker(OVTK_StimulationId_TrialStart);

        if (spelling_mode == CALIBRATION)
        {
            highlightTarget();
            text_row += desired_phrase[currentLetter];
            textRow->setText(text_row);
        }
        else if(spelling_mode == COPY_MODE)
        {
            highlightTarget();
        }
    }

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

void FlashingSpeller::feedback()
{
    qDebug() << Q_FUNC_INFO;

    receiveFeedback();
    textRow->setText(text_row);
    if (spelling_mode == COPY_MODE)
    {

        if(text_row[currentLetter - 1] == desired_phrase[currentLetter - 1])
        {
            this->layout()->itemAt(currentTarget)->
                    widget()->setStyleSheet("QLabel { color : green; font: 40pt }");
        }
        else
        {
            this->layout()->itemAt(currentTarget)->
                    widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");
        }

    }
    post_trial();
}

void FlashingSpeller::post_trial()
{
    qDebug()<< Q_FUNC_INFO;


    currentStimulation = 0;
    state = PRE_TRIAL;
    // wait
    wait(1000);

    refreshTarget();

    if (currentLetter >= desired_phrase.length() && (spelling_mode == COPY_MODE || spelling_mode == CALIBRATION)){
        qDebug()<< "Experiment End";
        sendMarker(OVTK_StimulationId_ExperimentStop);
        wait(2000);
        this->close();
    }
    else{
        startTrial();
    }

}

void FlashingSpeller::receiveFeedback()
{
    // wait for OV python script to write in UDP feedback socket
    wait(200);
    QHostAddress sender;
    quint16 senderPort;
    QByteArray *buffer = new QByteArray();

    buffer->resize(feedback_socket->pendingDatagramSize());
    qDebug() << "buffer size" << buffer->size();

    feedback_socket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
    feedback_socket->waitForBytesWritten();
    text_row += buffer->data();
    qDebug()<< "Feedback Data" << buffer->data();
}

bool FlashingSpeller::isTarget()
{
    int row, column;
    int index = flashingSequence->sequence[currentStimulation] - 1;
    row = index / nr_elements;
    column = index % nr_elements;

    if (desired_phrase[currentLetter]==letters[row][column])
    {
        // qDebug()<< "letter : " << letters[row][column];
        // qDebug()<< "desired letter: " << desired_phrase[currentLetter];
        // qDebug()<< "flashing: "<< flashingSequence->sequence[currentStimulation];
        // qDebug()<< "row: " << row << " column: "<< column;
        return true;
    }
    else
        return false;
}

void FlashingSpeller::highlightTarget()
{
    int idx = 0;

    for (int i=0; i<6; i++)
    {
        for (int j=0; j<6; j++)
        {
            idx++;
            if (desired_phrase[currentLetter] == letters[i][j]){
                currentTarget = idx;
                break;
            }
        }
    }
    this->layout()->itemAt(currentTarget)->
            widget()->setStyleSheet("QLabel { color : red; font: 60pt }");
}

void FlashingSpeller::refreshTarget()
{
    this->layout()->itemAt(currentTarget)->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
}

void FlashingSpeller::wait(int millisecondsToWait)
{

    qDebug()<< Q_FUNC_INFO;

    // from stackoverflow question:
    // http://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        //        qDebug()<<"waiting..."<<QTime::currentTime();
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100);

    }
}

/* Setters */
void FlashingSpeller::setSpeller_type(int value)
{
    speller_type = value;
}

void FlashingSpeller::setFeedbackPort(quint16 value)
{
    feedback_port = value;
}

void FlashingSpeller::setStimulation_duration(int value)
{
    stimulation_duration = value;
    stimTimer->setInterval(stimulation_duration);
}

void FlashingSpeller::setDesired_phrase(const QString &value)
{
    desired_phrase = value;
}

void FlashingSpeller::setSpelling_mode(int value)
{
    spelling_mode = value;
}

void FlashingSpeller::setNr_trials(int value)
{
    nr_trials = value;
}

void FlashingSpeller::setNr_sequence(int value)
{
    nr_sequence = value;
}

void FlashingSpeller::setIsi(int value)
{
    isi = value;
    isiTimer->setInterval(isi);
}

void FlashingSpeller::create_layout()
{
    // speller settings
    this->rows = 6;
    this->cols = 6;
    this->nr_elements = rows * cols;
    this->matrix_height = 640;
    this->matrix_width = 480;
    //    this->setGeometry(0, 0, matrix_width, matrix_height);
    //        this->Mlayout = new MatrixLayout(qMakePair(6,6), 6, 6);
    //    QHBoxLayout *genLayout = new QHBoxLayout();
    QGridLayout *layout = new QGridLayout();

    textRow = new QLabel(this);
    textRow->setText(desired_phrase);
    textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    textRow->setAlignment(Qt::AlignCenter);
    //    textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(textRow,0,0,1,0);

    // add speller ellements
    for(int i=1; i<rows+1; i++)
    {
        for(int j=0; j<cols; j++)
        {
            QLabel *element = new QLabel(this);
            element->setText(letters[i-1][j]);
            element->setStyleSheet("font: 40pt; color:gray");
            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element,i,j);
        }
    }

    this->setLayout(layout);
}

void FlashingSpeller::refresh_layout()
{

}

FlashingSpeller::~FlashingSpeller()
{
    delete ui;
}
