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
#include <QPixmap>
//
#include "hybridgridstimulation.h"
#include "ui_hybridgridstimulation.h"
#include "configpanel.h"
#include "ovtk_stimulations.h"
#include "randomflashsequence.h"
//
const quint8 PRE_TRIAL = 0;
const quint8 ERPSTIMULUS = 1;
const quint8 SSVEPSTIMULUS = 2;
const quint8 POST_ERPSTIMULUS = 3;
const quint8 FEEDBACK = 4;
const quint8 POST_TRIAL = 5;

const quint8 INVERTED_FACE = 6;
const quint8 COLORED_FACE = 7;
const quint8 INVERTED_COLORED_FACE = 8;

const quint8 CALIBRATION = 0;
const quint8 COPY_MODE  = 1;
const quint8 FREE_MODE = 2;

const uint64_t OVTK_StimulationLabel_Base = 0x00008100;
//
HybridGridStimulation::HybridGridStimulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HybridGridStimulation)
{
    ui->setupUi(this);

    this->show();
    this->setWindowTitle("Hybrid ERP/SSVEP BCI");

    if(qApp->screens().count() == 2){

        this->windowHandle()->setScreen(qApp->screens().last());
        this->showFullScreen();
    }

    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

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

    feedbackSocket = new QUdpSocket(this);
    feedbackSocket->bind(QHostAddress::LocalHost, feedbackPort);

    state = PRE_TRIAL;
}

void HybridGridStimulation::startTrial()
{
    qDebug()<< "[TRIAL START]" << Q_FUNC_INFO;

    if (state == PRE_TRIAL)
    {

        pre_trial();
    }

    if (state == ERPSTIMULUS)
    {
        startFlashing();

    }
    else if (state == POST_ERPSTIMULUS){

        pauseFlashing();

    }
    if(state == SSVEPSTIMULUS)
    {
        Flickering();
    }

}

void HybridGridStimulation::startFlashing()
{
    //    qDebug()<< Q_FUNC_INFO;

    sendMarker(OVTK_StimulationId_VisualStimulationStart);
    sendMarker(OVTK_StimulationLabel_Base + ERPFlashingSequence->sequence[currentStimulation]);

    // send target marker
    if (operationMode == CALIBRATION || operationMode == COPY_MODE)
    {
        if (isTarget()){sendMarker(OVTK_StimulationId_Target);}
        else{sendMarker(OVTK_StimulationId_NonTarget);}
    }

    if(ERPStimulationType == INVERTED_FACE)
    {
        //        if(ERPFlashingSequence->sequence[currentStimulation] == 5)
        //        {

        //            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]+1)->
        //                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_inverted.png);");
        //        }
        //        else
        //        {
        ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_inverted.png);");
        //        }
    }

    else if(ERPStimulationType == COLORED_FACE)
    {
        switch (ERPFlashingSequence->sequence[currentStimulation])
        {
        case 1:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_blue.png);");
            break;
        }
        case 2:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_green.png)");
            break;
        }
        case 3:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_red.png);");
            break;
        }
        case 4:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_cyan.png);");
            break;
        }
        case 5:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation] )->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face.png);" );
            break;
        }
        case 6:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_yellow.png)");
            break;
        }
        case 7:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_magenta.png);");
            break;
        }
        case 8:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_orange.png);");
            break;
        }
            //        case 9:
            //        {
            //            this->layout()->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
            //                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_magenta.png)");
            //            break;
            //        }

        }
    }

    else if(ERPStimulationType == INVERTED_COLORED_FACE)
    {
        switch (ERPFlashingSequence->sequence[currentStimulation])
        {
        case 1:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_blue_inverted.png);");
            break;
        }
        case 2:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_green_inverted.png);");
            break;
        }
        case 3:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_red_inverted.png);");
            break;
        }
        case 4:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_cyan_inverted.png);");
            break;
        }
        case 5:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_inverted.png);");
            break;
        }
        case 6:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_yellow_inverted.png);");
            break;
        }
        case 7:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_magenta_inverted.png);");
            break;
        }
        case 8:
        {
            ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_orange_inverted.png);");
            break;
        }
            //        case 9:
            //        {
            //            this->layout()->itemAt(ERPFlashingSequence->sequence[currentStimulation])->
            //                    widget()->setStyleSheet("image: url(:/images/assets/bennabi_face_magenta_inverted.png)");
            //            break;
            //        }

        }
    }

    stimTimer->start();
    isiTimer->stop();
    state = POST_ERPSTIMULUS;
}

void HybridGridStimulation::pauseFlashing()
{
    //    qDebug()<< Q_FUNC_INFO;
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);



    ERPLayout->itemAt(ERPFlashingSequence->sequence[currentStimulation]-1)->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");


    stimTimer->stop();
    isiTimer->start();
    //    qDebug("Isi Timer started");
    currentStimulation++;
    //    state = ERPSTIMULUS;

    if (currentStimulation >= ERPFlashingSequence->sequence.count())
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
        wait(1000);
        sendMarker(OVTK_StimulationId_TrialStop);
        //        state = FEEDBACK;

        state = SSVEPSTIMULUS;
        startTrial();

        if(operationMode == COPY_MODE || operationMode == FREE_MODE){
            feedback();
        }
        else if(operationMode == CALIBRATION)
        {
            post_trial();
        }
    }
}

void HybridGridStimulation::Flickering()
{
    qDebug()<<Q_FUNC_INFO;
    sendMarker(OVTK_StimulationId_VisualSteadyStateStimulationStart);

    QTime dieTime = QTime::currentTime().addMSecs(SSVEPStimulationDuration * 1000);
    QTime elapsedTime = QTime::currentTime();
    QTime elapsedTime1 = elapsedTime;
    QTime elapsedTime2 = elapsedTime;
    QTime elapsedTime3 = elapsedTime;

    int counter = 0;
    bool fl = false;
    bool fl1 = false;
    bool fl2 = false;
    bool fl3 = false;

    while( QTime::currentTime() < dieTime )

    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if( QTime::currentTime() == elapsedTime.addMSecs((1/frequencies[0])*500) )
        {
            elapsedTime = QTime::currentTime() ;
            if (!fl)
            {

                SSVEPLayout->itemAt(0)->widget()->setStyleSheet("background-color: black");
                fl = true;
            }
            else
            {
                SSVEPLayout->itemAt(0)->widget()->setStyleSheet("background-color: white");
                fl = false;
            }
        }
        else if(QTime::currentTime() == elapsedTime1.addMSecs((1/frequencies[1])*500))
        {

            elapsedTime1 = QTime::currentTime() ;
            if (!fl1)
            {
                SSVEPLayout->itemAt(1)->widget()->setStyleSheet("background-color: black");
                fl1 = true;
            }
            else
            {
                SSVEPLayout->itemAt(1)->widget()->setStyleSheet("background-color: white");
                fl1 = false;
            }
        }
        else if(QTime::currentTime() == elapsedTime2.addMSecs((1/frequencies[2])*500))
        {
            elapsedTime2 = QTime::currentTime() ;
            if (!fl2)
            {
                SSVEPLayout->itemAt(3)->widget()->setStyleSheet("background-color: black");
                fl2 = true;
            }
            else
            {
                SSVEPLayout->itemAt(3)->widget()->setStyleSheet("background-color: white");
                fl2 = false;
            }
        }
        else if(QTime::currentTime() == elapsedTime3.addMSecs((1/frequencies[3])*500))
        {
            elapsedTime3 = QTime::currentTime() ;
            if (!fl3)
            {
                SSVEPLayout->itemAt(4)->widget()->setStyleSheet("background-color: black");
                fl3 = true;
            }
            else
            {
                SSVEPLayout->itemAt(4)->widget()->setStyleSheet("background-color: white");
                fl3 = false;
            }
        }

    }

    sendMarker(OVTK_StimulationId_VisualSteadyStateStimulationStop);
    state = POST_TRIAL;

    if(operationMode == COPY_MODE || operationMode == FREE_MODE){
        feedback();
    }
    else if(operationMode == CALIBRATION)
    {
        post_trial();
    }
}

void HybridGridStimulation::pre_trial()
{
    qDebug()<< Q_FUNC_INFO;

    if (pre_trial_count == 0)
    {
        ERPFlashingSequence = new RandomFlashSequence(nrERPElements, nrERPSequence);

        qDebug()<<"Sequence"<<ERPFlashingSequence->sequence;
        //                SSVEPFlickeringSequence = new RandomFlashSequence(nrSSVEPElements, 1);
        sendMarker(OVTK_StimulationId_TrialStart);

        if (operationMode == CALIBRATION)
        {
            qDebug()<< "highlightTarget";
            highlightTarget();
            text_row += desiredSequence[currentLetter];
            textRow->setText(text_row);
        }
        else if(operationMode == COPY_MODE)
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
        state = ERPSTIMULUS;
    }

}

void HybridGridStimulation::feedback()
{
    qDebug() << Q_FUNC_INFO;

    receiveFeedback();
    textRow->setText(text_row);
    if (operationMode == COPY_MODE)
    {

        if(text_row[currentLetter - 1] == desiredSequence[currentLetter - 1])
        {
            ERPLayout->layout()->itemAt(currentTarget)->
                    widget()->setStyleSheet("QLabel { color : green; font: 40pt }");
        }
        else
        {
            ERPLayout->layout()->itemAt(currentTarget)->
                    widget()->setStyleSheet("QLabel { color : blue; font: 40pt }");
        }

    }
    post_trial();
}

void HybridGridStimulation::post_trial()
{
    qDebug()<< Q_FUNC_INFO;


    currentStimulation = 0;
    state = PRE_TRIAL;
    // wait
    wait(1000);

    refreshTarget();

    if (currentLetter >= desiredSequence.length() && (operationMode == COPY_MODE || operationMode == CALIBRATION))
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

void HybridGridStimulation::receiveFeedback()
{
    // wait for OV python script to write in UDP feedback socket
    wait(200);
    QHostAddress sender;
    quint16 senderPort;
    QByteArray *buffer = new QByteArray();

    buffer->resize(feedbackSocket->pendingDatagramSize());
    qDebug() << "buffer size" << buffer->size();

    feedbackSocket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
    feedbackSocket->waitForBytesWritten();
    text_row += buffer->data();
    qDebug()<< "Feedback Data" << buffer->data();
}

bool HybridGridStimulation::isTarget()
{
    //    qDebug()<<Q_FUNC_INFO;
    int index = ERPFlashingSequence->sequence[currentStimulation] - 1;
    if(desiredSequence[currentLetter]==presentedCommands[index][0])
    {
        //                qDebug()<< "letter : " << letters[row][column];
        //        qDebug()<< "desired letter: " << desiredSequence[currentLetter];
        //        qDebug()<< "flashing: "<< flashingSequence->sequence[currentStimulation];
        //        qDebug()<< "presented letter:" << desiredSequence[index];
        //        qDebug()<< "row: " << row << " column: "<< column;
        return true;
    }
    else
    {
        return false;
    }

}

void HybridGridStimulation::highlightTarget()
{
    int idx = 0;

    for (int i=0; i<nrERPElements; i++)
    {
        if(desiredSequence[currentLetter]==presentedCommands[idx][0])
        {
            currentTarget = idx;
            break;
        }
        idx++;
    }

    if ( (ERPStimulationType == COLORED_FACE) || (ERPStimulationType == INVERTED_COLORED_FACE) )
    {
        switch (currentTarget)
        {
        case 0:
        {
            ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : blue; font: 60pt }");
            break;
        }
        case 1:
        {
            ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : green; font: 60pt }");
            break;
        }
        case 2:
        {
            ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : red; font: 60pt }");
            break;
        }
        case 3:
        {
            ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : cyan; font: 60pt }");
            break;
        }
        case 4:
        {
            ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : white; font: 60pt }");
            break;
        }
        case 5:
        {
            ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : yellow; font: 60pt }");
            break;
        }
        case 6:
        {
            ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : magenta; font: 60pt }");
            break;
        }
        case 7:
        {
            ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : orange; font: 60pt }");
            break;
        }

        }
    }
    else
        ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : red; font: 60pt }");
}

void HybridGridStimulation::refreshTarget()
{

    ERPLayout->itemAt(currentTarget)->widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

}

void HybridGridStimulation::wait(int millisecondsToWait)
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
void HybridGridStimulation::setERPStimulationType(int value)
{
    ERPStimulationType = value;
}

void HybridGridStimulation::setFrequencies(QString freqs)
{
    QStringList freqsList = freqs.split(',');

    foreach(QString str, freqsList)
    {

        frequencies.append(str.toFloat());
    }
}

void HybridGridStimulation::setSSVEPStimulationDuration(int stimDuration)
{
    SSVEPStimulationDuration = stimDuration;
}

void HybridGridStimulation::setFeedbackPort(quint16 value)
{
    feedbackPort = value;
}

void HybridGridStimulation::setERPStimulationDuration(int value)
{
    ERPstimulationDuration = value;
    stimTimer->setInterval(ERPstimulationDuration);
}

void HybridGridStimulation::setDesiredSequence(const QString &value)
{
    desiredSequence = value;
}

void HybridGridStimulation::setOperationMode(int value)
{
    operationMode = value;
}

void HybridGridStimulation::setERPNrtrials(int value)
{
    nrERPTrials = value;
}

void HybridGridStimulation::setERPNrSequence(int value)
{
    nrERPSequence = value;
}

void HybridGridStimulation::setIsi(int value)
{
    isi = value;
    isiTimer->setInterval(isi);
}

void HybridGridStimulation::create_layout()
{
    nrERPElements = 8;
    nrCols = 3;
    nrRows = 3;
    nrSSVEPElements = 5;
    ERPLayout = new QGridLayout;
    SSVEPLayout = new QGridLayout;
    //    QGridLayout *layout = new QGridLayout;

    textRow = new QLabel();
    textRow->setText(desiredSequence);
    textRow->setStyleSheet("font:30pt; color:gray; border-color:white;");
    textRow->setAlignment(Qt::AlignCenter);

    // freq 1
    QLabel *element1 = new QLabel(this);
    element1->setStyleSheet("background-color: rgb(255, 255, 255)");
    element1->setAlignment(Qt::AlignCenter);
    element1->setMaximumSize(QSize(100, 100));
    SSVEPLayout->addWidget(element1,0,0);

    //freq 2 3 4
    QLabel *element2 = new QLabel(this);
    element2->setStyleSheet("background-color: rgb(255, 255, 255)");
    element2->setMaximumSize(QSize(100, 100));
    element2->setAlignment(Qt::AlignCenter);
    SSVEPLayout->addWidget(element2,0,4);

    QLabel *element3 = new QLabel(this);
    element3->setStyleSheet("background-color: gray");
    element3->setMaximumSize(QSize(100, 100));
    element3->setAlignment(Qt::AlignCenter);
    SSVEPLayout->addWidget(element3,1,1);


    QLabel *element4 = new QLabel(this);
    element4->setStyleSheet("background-color: rgb(255, 255, 255)");
    element4->setMaximumSize(QSize(100, 100));
    element4->setAlignment(Qt::AlignCenter);
    SSVEPLayout->addWidget(element4,3,0);

    //freq5
    QLabel *element5 = new QLabel(this);
    element5->setStyleSheet("background-color: rgb(255, 255, 255)");
    element5->setAlignment(Qt::AlignCenter);
    element5->setMaximumSize(QSize(100, 100));
    SSVEPLayout->addWidget(element5,3,4);


    int k=1;
    for (int i=1;i<nrRows+1;i++)
    {
        for (int j=0;j<nrCols ;j++)
        {
            if (i != 2 || j!=1)
            {
                QLabel *element = new QLabel(this);
                element->setText(QString::number(k));
                element->setStyleSheet("font: 40pt; color:gray");
                element->setAlignment(Qt::AlignCenter);
                ERPLayout->addWidget(element,i,j);
                presentedCommands.append(QString::number(k));
                k++;
            }
        }
    }


    ERPLayout->addWidget(textRow,0,0,1,0);
    ERPLayout->addLayout(SSVEPLayout,2,1);
    this->setLayout(ERPLayout);
}

void HybridGridStimulation::refresh_layout()
{

}

HybridGridStimulation::~HybridGridStimulation()
{
    delete ui;
}

