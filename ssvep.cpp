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
//
#include "ssvep.h"
#include "ui_ssvep.h"
//#include "randomflashsequence.h"
#include "ovtk_stimulations.h"
//
const quint8 CALIBRATION = 0;
const quint8 COPY_MODE  = 1;
const quint8 FREE_MODE = 2;
const quint8 SIGNLE  = 3;
//
const quint8 PRE_TRIAL = 0;
const quint8 STIMULUS = 1;
const quint8 POST_STIMULUS = 2;
const quint8 FEEDBACK = 3;
const quint8 POST_TRIAL = 4;
//
const uint64_t OVTK_StimulationLabel_Base = 0x00008100;
//
extern "C" NTSYSAPI NTSTATUS NTAPI NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);
ULONG currentRes;

//
Ssvep::Ssvep(quint8 nrElements, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ssvep)
{
    qDebug()<<Q_FUNC_INFO;
    //NtSetTimerResolution(5000, TRUE, &currentRes);
    ui->setupUi(this);
    this->show();

    if(qApp->screens().count() == 2){

        this->windowHandle()->setScreen(qApp->screens().last());
        this->showFullScreen();
    }

    //  this->windowHandle()->setScreen(qApp->screens().last());
    // this->showFullScreen();
    this->setStyleSheet("background-color : black");

    nr_elements = nrElements;

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
    //  qDebug()<< "[TRIAL START]" << Q_FUNC_INFO;

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
    //  qDebug()<< Q_FUNC_INFO;

    if(firstRun)
    {
        qDebug()<< nr_elements << stimulationSequence << stimulationSequence / nr_elements;
        flickeringSequence = new RandomFlashSequence(nr_elements, stimulationSequence / nr_elements);
        qDebug()<<"sequence"<<flickeringSequence->sequence;
        firstRun = false;
    }

    if (pre_trial_count == 0)
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

void Ssvep::post_trial()
{
    //  qDebug()<< Q_FUNC_INFO;


    sendMarker(OVTK_StimulationId_TrialStop);
    //    currentStimulation = 0;
    state = PRE_TRIAL;
    // wait
    int waitMillisec = breakDuration - pre_trial_wait * 1000;
    wait(waitMillisec);

    //    refreshTarget();

    //    qDebug()<<"current flicker"<<currentFlicker;
    //    qDebug()<<"sequence size"<< flickeringSequence->sequence.size();

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

void Ssvep::Flickering()
{
    qDebug()<< Q_FUNC_INFO;

    sendMarker(OVTK_StimulationLabel_Base + flickeringSequence->sequence[currentFlicker]);
    sendMarker(OVTK_StimulationId_VisualSteadyStateStimulationStart);

    QTime elapsedTime = QTime::currentTime();
    QTime elapsedTime1 = elapsedTime;
    QTime elapsedTime2 = elapsedTime;
    QTime elapsedTime3 = elapsedTime;
    QTime t;
    QElapsedTimer timer;
    // qDebug()<< timer.clockType();
    qint64 nanoSec;
    int counter = 0;
    int counter1 = 0;
    int counter2 = 0;
    int counter3 = 0;
    bool fl = false;
    bool fl1 = false;
    bool fl2 = false;
    bool fl3 = false;
    timer.start();
    int c = 0;

    t = QTime::currentTime();

    QTime dieTime = t.addMSecs(qRound (stimulationDuration * 1000) );

    while( t <= dieTime)
    {
        // t =  QTime::currentTime();
        c++;
        //        qDebug()<<"elapsed"<< elapsedTime.elapsed();
        //        qDebug()<<"Flickering"<<QTime::currentTime();
        //        QCoreApplication::processEvents( QEventLoop::AllEvents, 100);
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (nr_elements == 1)
        {
            if( QTime::currentTime() == elapsedTime.addMSecs(qRound((1/frequencies[0])*500)) )
            {
                // elapsedTime = QTime::currentTime();
                elapsedTime.restart();
                if (!fl)
                {
                    this->layout()->itemAt(0)->widget()
                            ->setStyleSheet("background-color: black");
                    fl = true;
                }
                else
                {
                    this->layout()->itemAt(0)->widget()
                            ->setStyleSheet("background-color: white");
                    fl = false;

                }
            }
        }


        else
        {

            if( t == elapsedTime.addMSecs(qFloor((1/frequencies[0])*500)) )
            {
                //            qDebug()<<"freq1 dur"<< (1/frequencies[0])*1000;
                // qDebug()<<"Freq1 flick"<< timer.nsecsElapsed();
                // elapsedTime = QTime::currentTime();
                elapsedTime.restart();
                //            qDebug()<< "elapsed time"<< elapsedTime;
                if (!fl)
                {this->layout()->itemAt(0)->widget()
                            ->setStyleSheet("background-color: black");
                    //                qDebug()<<"BLACK";
                    fl = true;
                }
                else
                {  this->layout()->itemAt(0)->widget()
                            ->setStyleSheet("background-color: white");
                    fl = false;
                    //                qDebug()<<"White";

                }


                counter++;
            }
            else if(t == elapsedTime1.addMSecs(qFloor((1/frequencies[1])*500)))
            {

                elapsedTime1 = QTime::currentTime() ;

                if (!fl1)
                {this->layout()->itemAt(1)->widget()
                            ->setStyleSheet("background-color: black");
                    //                qDebug()<<"BLACK";
                    fl1 = true;
                }
                else
                {  this->layout()->itemAt(1)->widget()
                            ->setStyleSheet("background-color: white");
                    fl1 = false;
                    //                qDebug()<<"White";

                }

                counter1++;
            }

            else if(t == elapsedTime2.addMSecs(qFloor((1/frequencies[2])*500)))
            {
                elapsedTime2 = QTime::currentTime() ;

                if (!fl2)
                {this->layout()->itemAt(3)->widget()
                            ->setStyleSheet("background-color: black");
                    //                qDebug()<<"BLACK";
                    fl2 = true;
                }
                else
                {  this->layout()->itemAt(3)->widget()
                            ->setStyleSheet("background-color: white");
                    fl2 = false;
                    //                qDebug()<<"White";

                }

                counter2++;
            }

            else if(t == elapsedTime3.addMSecs(qFloor   ((1/frequencies[3])*500)))
            {
                //  elapsedTime3 = QTime::currentTime() ;

                if (!fl3)
                {
                    this->layout()->itemAt(4)->widget()->setStyleSheet("background-color: black");
                    // qDebug()<<"BLACK";
                    fl3 = true;

                }
                else
                {  this->layout()->itemAt(4)->widget()->setStyleSheet("background-color: white");

                    fl3 = false;
                    //qDebug()<<"White";
                }
                elapsedTime3.restart();
                counter3++;
            }

            //        qDebug()<<"Flickering"<<QTime::currentTime();
            //QCoreApplication::processEvents( QEventLoop::AllEvents, 100);
            //Flickering
            // refresh layout
        }
        //        t = QTime::currentTime();
        t.restart();
    }
    qDebug()<<"die_time "<<dieTime;
    qDebug()<<"final t "<<t;
    // timer.restart();
    qDebug()<<"freq4 counter="<<counter;
    qDebug()<<"freq4 counter="<<counter1;
    qDebug()<<"freq4 counter="<<counter2;
    qDebug()<<"freq4 counter="<<counter3;
    qDebug()<<"iteration counter= "<<c;

    /*
    qDebug()<<"0 increment by: "<<qFloor((1/frequencies[0])*500);
    qDebug()<<"1 increment by: "<<qFloor((1/frequencies[1])*500);
    qDebug()<<"2 increment by: "<<qFloor((1/frequencies[2])*500);
    qDebug()<<"3 increment by: "<<qFloor((1/frequencies[3])*500);
    */

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

    //    nr_elements = 5;

    QGridLayout *layout = new QGridLayout();
    //    QHBoxLayout *layout = new QHBoxLayout();

    //    textRow = new QLabel(this);
    //    textRow->setText("desired_phrase");
    //    textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    //    textRow->setAlignment(Qt::AlignCenter);
    //    textRow->setMaximumSize(QSize(500, 100));
    ////        textRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //    layout->addWidget(textRow,0,0,1,0);

    if (nr_elements == 1)
    {

        QLabel *element = new QLabel(this);
        element->setStyleSheet("background-color: white");
        element->setMaximumSize(QSize(100, 100));
        element->setAlignment(Qt::AlignCenter);
        layout->addWidget(element,1,1);
    }
    else
    {
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
    }
    this->setLayout(layout);


}

void Ssvep::highlightTarget()
{
    //    qDebug()<<Q_FUNC_INFO<<flickeringSequence->sequence[currentFlicker];

    if (nr_elements > 1)
    {

        if(flickeringSequence->sequence[currentFlicker]==3)
            this->layout()->itemAt(2)
                ->widget()->setStyleSheet("background-color: gray; border: 4px solid red");
        else
            this->layout()->itemAt(flickeringSequence->sequence[currentFlicker]-1)
                ->widget()->setStyleSheet("background-color: rgb(255, 255, 255); border: 4px solid red");
    }
}

void Ssvep::refreshTarget()
{
    //    qDebug()<<Q_FUNC_INFO<<flickeringSequence->sequence[currentFlicker];
    if (nr_elements > 1)
    {
        if(flickeringSequence->sequence[currentFlicker]==3)
        {
            qDebug()<<"refresh gray box";
            this->layout()->itemAt(2)
                    ->widget()->setStyleSheet("background-color: gray");
        }
        else
        {
            qDebug()<<"refresh white box";
            this->layout()->itemAt(flickeringSequence->sequence[currentFlicker]-1)
                    ->widget()->setStyleSheet("background-color: white");
        }
    }
}

//Setters

void Ssvep::setFrequencies(QString freqs)
{
    QStringList freqsList = freqs.split(',');

    if (nr_elements == 1)
    {
        frequencies[0] = freqsList[0].toFloat();
    }
    else
    {
        foreach(QString str, freqsList)
        {

            frequencies.append(str.toFloat());
        }
    }

}

void Ssvep::setFlickeringMode(int mode)
{
    flickering_mode = mode;

}

void Ssvep::setStimulationDuration(float stimDuration)
{
    stimulationDuration = stimDuration;

}

void Ssvep::setBreakDuration(int brkDuration)
{
    breakDuration = brkDuration;
    pre_trial_wait = brkDuration;

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
