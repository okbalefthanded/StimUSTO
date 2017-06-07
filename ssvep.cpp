//
#include <QWindow>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTime>
//
#include "ssvep.h"
#include "ui_ssvep.h"
//#include "randomflashsequence.h"
#include "ovtk_stimulations.h"
//
const quint8 CALIBRATION = 0;
const quint8 COPY_MODE  = 1;
const quint8 FREE_MODE = 2;
//
const quint8 PRE_TRIAL = 0;
const quint8 STIMULUS = 1;
const quint8 POST_STIMULUS = 2;
const quint8 FEEDBACK = 3;
const quint8 POST_TRIAL = 4;
//
const uint64_t OVTK_StimulationLabel_Base = 0x00008100;
//
Ssvep::Ssvep(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ssvep)
{
    qDebug()<<Q_FUNC_INFO;

    ui->setupUi(this);
    this->show();

    this->windowHandle()->setScreen(qApp->screens().last());
    this->showFullScreen();
    this->setStyleSheet("background-color : black");

    create_layout();

    preTrialTimer = new QTimer(this);
    preTrialTimer->setTimerType(Qt::PreciseTimer);
    preTrialTimer->setInterval(1000);
    preTrialTimer->setSingleShot(true);
    connect( preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );

    feedback_socket = new QUdpSocket(this);
    feedback_socket->bind(QHostAddress::LocalHost, feedbackPort);

    state = PRE_TRIAL;
}


void Ssvep::startTrial()
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
void Ssvep::pre_trial()
{
    qDebug()<< Q_FUNC_INFO;

    if(firstRun)
    {
        flickeringSequence = new RandomFlashSequence(nr_elements, stimulationSequence);
        qDebug()<<"sequence"<<flickeringSequence->sequence;
        firstRun = false;
    }

    if (pre_trial_count == 0)
    {

        sendMarker(OVTK_StimulationId_TrialStart);

        if (flickering_mode == CALIBRATION)
        {
            qDebug()<< "highlightTarget";
            highlightTarget();
            //            text_row += desired_phrase[currentLetter];
            //            textRow->setText(text_row);
        }
        else if(flickering_mode == COPY_MODE)
        {
            highlightTarget();
        }
    }
    qDebug()<< "Pre trial timer start";

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

void Ssvep::post_trial()
{
    qDebug()<< Q_FUNC_INFO;


    //    currentStimulation = 0;
    state = PRE_TRIAL;
    // wait
    int waitMillisec = breakDuration - pre_trial_wait * 1000;
    wait(waitMillisec);

    refreshTarget();

    if (currentFlicker >= flickeringSequence->sequence.size() && (flickering_mode == COPY_MODE || flickering_mode == CALIBRATION))
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

void Ssvep::Flickering()
{
    qDebug()<< Q_FUNC_INFO;

    sendMarker(OVTK_StimulationLabel_Base + flickeringSequence->sequence[currentFlicker]);

    QTime dieTime = QTime::currentTime().addMSecs(stimulationDuration * 1000);
    QTime elapsedTime = QTime::currentTime();
    while( QTime::currentTime() < dieTime )
    {

        qDebug()<<"Flickering"<<QTime::currentTime();
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100);
        //Flickering



    }

    currentFlicker++;
//    qDebug()<<"current flicker"<<currentFlicker;
    state = POST_TRIAL;

}

void Ssvep::wait(int millisecondsToWait)
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
void Ssvep::create_layout()
{
    qDebug()<<Q_FUNC_INFO;

    nr_elements = 5;

    QGridLayout *layout = new QGridLayout();
    //    QHBoxLayout *layout = new QHBoxLayout();

    //    textRow = new QLabel(this);
    //    textRow->setText("desired_phrase");
    //    textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    //    textRow->setAlignment(Qt::AlignCenter);
    //    textRow->setMaximumSize(QSize(500, 100));
    ////        textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //    layout->addWidget(textRow,0,0,1,0);

    // freq1
    QLabel *element1 = new QLabel(this);
    element1->setStyleSheet("background-color: rgb(255, 255, 255)");
    element1->setAlignment(Qt::AlignCenter);
    element1->setMaximumSize(QSize(100, 100));
    layout->addWidget(element1,0,1);

    //freq 2 3 4
    QLabel *element2 = new QLabel(this);
    element2->setStyleSheet("background-color: rgb(255, 255, 255)");
    element2->setMaximumSize(QSize(100, 100));
    element2->setAlignment(Qt::AlignCenter);
    layout->addWidget(element2,1,0);

    QLabel *element3 = new QLabel(this);
    element3->setStyleSheet("background-color: gray");
    element3->setMaximumSize(QSize(100, 100));
    element3->setAlignment(Qt::AlignCenter);
    layout->addWidget(element3,1,1);

    QLabel *element4 = new QLabel(this);
    element4->setStyleSheet("background-color: rgb(255, 255, 255)");
    element4->setMaximumSize(QSize(100, 100));
    element4->setAlignment(Qt::AlignCenter);
    layout->addWidget(element4,1,2);

    //freq5
    QLabel *element5 = new QLabel(this);
    element5->setStyleSheet("background-color: rgb(255, 255, 255)");
    element5->setAlignment(Qt::AlignCenter);
    element5->setMaximumSize(QSize(100, 100));
    layout->addWidget(element5,2,1);

    this->setLayout(layout);

}

//bool Ssvep::isTarget()
//{

//}

void Ssvep::highlightTarget()
{
    qDebug()<<Q_FUNC_INFO;
    qDebug()<<flickeringSequence->sequence[currentFlicker];

    if(flickeringSequence->sequence[currentFlicker]==3)
        this->layout()->itemAt(flickeringSequence->sequence[currentFlicker]-1)
            ->widget()->setStyleSheet("background-color: gray; border: 2px solid red");
    else
        this->layout()->itemAt(flickeringSequence->sequence[currentFlicker]-1)
            ->widget()->setStyleSheet("background-color: rgb(255, 255, 255); border: 2px solid red");
}

void Ssvep::refreshTarget()
{
    if(flickeringSequence->sequence[currentFlicker]==3)
        this->layout()->itemAt(flickeringSequence->sequence[currentFlicker]-1)
            ->widget()->setStyleSheet("background-color: gray");
    else
        this->layout()->itemAt(flickeringSequence->sequence[currentFlicker]-1)
            ->widget()->setStyleSheet("background-color: rgb(255, 255, 255)");
}

//Setters

void Ssvep::setFrequencies(QString freqs)
{
    QStringList freqsList = freqs.split(',');

    foreach(QString str, freqsList)
    {

        frequencies.append(str.toFloat());
    }


}

void Ssvep::setFlickeringMode(int mode)
{
    flickering_mode = mode;

}

void Ssvep::setStimulationDuration(int stimDuration)
{
    stimulationDuration = stimDuration;

}

void Ssvep::setBreakDuration(int brkDuration)
{
    breakDuration = brkDuration;

}

void Ssvep::setSequence(int sequence)
{
    stimulationSequence = sequence;

}

void Ssvep::setFeedbackPort(int port)
{
    feedbackPort = port;
}

//
Ssvep::~Ssvep()
{
    delete ui;
}
