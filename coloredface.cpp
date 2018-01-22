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
//
#include "coloredface.h"
#include "ui_coloredface.h"
#include "configpanel.h"
#include "ovtk_stimulations.h"
#include "randomflashsequence.h"
//
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
const quint8 INVERTED_FACE = 6;
const quint8 COLORED_FACE = 7;
const quint8 INVERTED_COLORED_FACE = 8;

const quint8 CALIBRATION = 0;
const quint8 COPY_MODE  = 1;
const quint8 FREE_MODE = 2;

const uint64_t OVTK_StimulationLabel_Base = 0x00008100;
//
ColoredFace::ColoredFace(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColoredFace)
{
    ui->setupUi(this);

    this->show();

    if(qApp->screens().count() == 2){

        this->windowHandle()->setScreen(qApp->screens().last());
        this->showFullScreen();
    }

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

void ColoredFace::startTrial()
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

}

void ColoredFace::startFlashing()
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

    if(speller_type == COLORED_FACE)
    {
        switch (flashingSequence->sequence[currentStimulation])
        {
        case 1:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_blue.png)");
            break;
        }
        case 2:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_green.png)");
            break;
        }
        case 3:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_red.png)");
            break;
        }
        case 4:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_brown.png)");
            break;
        }
        case 5:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_cyan.png)");
            break;
        }
        case 6:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face.png)");
            break;
        }
        case 7:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_yellow.png)");
            break;
        }
        case 8:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_orange.png)");
            break;
        }
        case 9:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_magenta.png)");
            break;
        }

        }

        //          this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
        //                widget()->setStyleSheet("image: url(:/images/bennabi_face.png)");
    }

    else if(speller_type == INVERTED_COLORED_FACE)
    {
        switch (flashingSequence->sequence[currentStimulation])
        {
        case 1:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_blue_inverted.png)");
            break;
        }
        case 2:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_green_inverted.png)");
            break;
        }
        case 3:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_red_inverted.png)");
            break;
        }
        case 4:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_brown_inverted.png)");
            break;
        }
        case 5:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_cyan_inverted.png)");
            break;
        }
        case 6:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");
            break;
        }
        case 7:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_yellow_inverted.png)");
            break;
        }
        case 8:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_orange_inverted.png)");
            break;
        }
        case 9:
        {
            this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
                    widget()->setStyleSheet("image: url(:/images/bennabi_face_magenta_inverted.png)");
            break;
        }

        }
        //        this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
        //                widget()->setStyleSheet("image: url(:/images/bennabi_face_inverted.png)");
    }


    stimTimer->start();

    //    qDebug("Stim Timer started");
    isiTimer->stop();
    state = POST_STIMULUS;
}

void ColoredFace::pauseFlashing()
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
        wait(1000);
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

void ColoredFace::pre_trial()
{
    qDebug()<< Q_FUNC_INFO;

    if (pre_trial_count == 0)
    {
        flashingSequence = new RandomFlashSequence(nr_elements, nr_sequence);
        sendMarker(OVTK_StimulationId_TrialStart);

        if (spelling_mode == CALIBRATION)
        {
            qDebug()<< "highlightTarget";
            highlightTarget();
            text_row += desired_phrase[currentLetter];
            textRow->setText(text_row);
        }
        else if(spelling_mode == COPY_MODE)
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

void ColoredFace::feedback()
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

void ColoredFace::post_trial()
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

void ColoredFace::receiveFeedback()
{
    // wait for OV python script to write in UDP feedback socket
    wait(200);
    QHostAddress sender;
    quint16 senderPort;
    QByteArray *buffer = new QByteArray();

    buffer->resize(feedback_socket->pendingDatagramSize());
    qDebug() << "buffer size" << buffer->size();

    feedback_socket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
//    feedback_socket->waitForBytesWritten();
    text_row += buffer->data();
    qDebug()<< "Feedback Data" << buffer->data();
}

bool ColoredFace::isTarget()
{
    //    int row, column;
    int index = flashingSequence->sequence[currentStimulation] - 1;
    //    row = index / nr_elements;
    //    column = index % nr_elements;


    //    if(desired_phrase[currentLetter]==presented_letters[index])
    //    {
    //        //        qDebug()<< "letter : " << letters[row][column];
    //        //        qDebug()<< "desired letter: " << desired_phrase[currentLetter];
    //        //        qDebug()<< "flashing: "<< flashingSequence->sequence[currentStimulation];
    //        //        qDebug()<< "presented letter:" << presented_letters[index];
    //        //        qDebug()<< "row: " << row << " column: "<< column;
    //        return true;
    //    }
    //    else
    //    {
    //        return false;
    //    }

    if(desired_phrase[currentLetter]==presented_letters[index][0])
    {
        //                qDebug()<< "letter : " << letters[row][column];
        qDebug()<< "desired letter: " << desired_phrase[currentLetter];
        //        qDebug()<< "flashing: "<< flashingSequence->sequence[currentStimulation];
        qDebug()<< "presented letter:" << presented_letters[index];
        //        qDebug()<< "row: " << row << " column: "<< column;
        return true;
    }
    else
    {
        return false;
    }

}

void ColoredFace::highlightTarget()
{
    int idx = 0;

    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<cols; j++)
        {
            //                        idx++;
            //            if (desired_phrase[currentLetter] == letters[i][j]){
            //                currentTarget = idx;
            //                break;
            if(desired_phrase[currentLetter]==presented_letters[idx][0])
            {
                currentTarget = idx + 1;
                break;
            }
            idx++;
        }
    }
    //    qDebug()

//    this->layout()->itemAt(currentTarget)->
//            widget()->setStyleSheet("QLabel { color : red; font: 60pt }");
    switch (currentTarget)
    {
    case 1:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : blue; font: 60pt }");
        break;
    }
    case 2:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : green; font: 60pt }");
        break;
    }
    case 3:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : red; font: 60pt }");
        break;
    }
    case 4:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : brown; font: 60pt }");
        break;
    }
    case 5:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : cyan; font: 60pt }");
        break;
    }
    case 6:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : white; font: 60pt })");
        break;
    }
    case 7:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : yellow; font: 60pt }");
        break;
    }
    case 8:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : orange; font: 60pt }");
        break;
    }
    case 9:
    {
        this->layout()->itemAt(currentTarget)->
                widget()->setStyleSheet("QLabel { color : magenta; font: 60pt }");
        break;
    }

    }
}

void ColoredFace::refreshTarget()
{
    this->layout()->itemAt(currentTarget)->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");
}

void ColoredFace::wait(int millisecondsToWait)
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
void ColoredFace::setSpeller_type(int value)
{
    speller_type = value;
}

void ColoredFace::setFeedbackPort(quint16 value)
{
    feedback_port = value;
}

void ColoredFace::setStimulation_duration(int value)
{
    stimulation_duration = value;
    stimTimer->setInterval(stimulation_duration);
}

void ColoredFace::setDesired_phrase(const QString &value)
{
    desired_phrase = value;
}

void ColoredFace::setSpelling_mode(int value)
{
    spelling_mode = value;
}

void ColoredFace::setNr_trials(int value)
{
    nr_trials = value;
}

void ColoredFace::setNr_sequence(int value)
{
    nr_sequence = value;
}

void ColoredFace::setIsi(int value)
{
    isi = value;
    isiTimer->setInterval(isi);
}

void ColoredFace::create_layout()
{
    // speller settings
    this->rows = 3;
    this->cols = 3;
    this->nr_elements = rows * cols;
    //    this->matrix_height = 640;
    //    this->matrix_width = 480;
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

    int k = 1;
    // add speller ellements
    for(int i=1; i<rows+1; i++)
    {
        for(int j=0; j<cols; j++)
        {
            QLabel *element = new QLabel(this);
            //            element->setText(letters[i-1][j]);
            element->setText(QString::number(k));
            element->setStyleSheet("font: 40pt; color:gray");
            element->setAlignment(Qt::AlignCenter);
            layout->addWidget(element,i,j);
            presented_letters.append(QString::number(k));
            k++;
            //            presented_letters.append(letters[i-1][j]);
        }
    }

    this->setLayout(layout);
}

void ColoredFace::refresh_layout()
{

}

ColoredFace::~ColoredFace()
{
    delete ui;
}
