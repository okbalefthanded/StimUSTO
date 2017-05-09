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

QChar letters[6][6] = {
    {'A','B','C','D','E','F'},
    {'G','H','I','J','K','L'},
    {'M','N','O','Q','P','R'},
    {'S','T','U','V','W','X'},
    {'Y','Z','0','1','2','3'},
    {'4','5','6','7','8','9'},
};

const uint PRE_TRIAL = 0;
const uint STIMULUS = 1;
const uint POST_STIMULUS = 2;
const uint FEEDBACK = 3;
const uint POST_TRIAL = 4;

const uint FLASHING_SPELLER = 0;
const uint FACES_SPELLER = 1;

const int CALIBRATION = 0;
const int COPY_MODE  = 1;
const int FREE_MODE = 2;

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

    state = PRE_TRIAL;

}

//TODO : define tick loop
void FlashingSpeller::tick()
{

    qDebug()<< "[TICK]";

    while (running){
        if (state == PRE_TRIAL){
            //                pre_trial();
        }
        else if (state == STIMULUS){
            startTrial();
        }
        else if (state == FEEDBACK){
            qDebug()<<"doing feedba9";
            //                feedback();
        }
        else if (state == POST_TRIAL){
            qDebug()<<"doing post trial";
            //                post_trial();
        }
    }
}

void FlashingSpeller::wait(int millisecondsToWait)
{

    qDebug()<< Q_FUNC_INFO;

    // from stackoverflow question:
    // http://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}


//TODO: start flashing
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
    qDebug()<< Q_FUNC_INFO;

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
    else
        this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
            widget()->setStyleSheet("QLabel { color : white; font: 60pt }");

    stimTimer->start();
    qDebug("Stim Timer started");
    isiTimer->stop();
    state = POST_STIMULUS;
}

void FlashingSpeller::pauseFlashing()
{
    qDebug()<< Q_FUNC_INFO;
    // sendMarker(OVTK_StimulationId_VisualStimulationStop);
    this->layout()->itemAt(flashingSequence->sequence[currentStimulation])->
            widget()->setStyleSheet("QLabel { color : gray; font: 40pt }");

    stimTimer->stop();
    isiTimer->start();
    qDebug("Isi Timer started");
    currentStimulation++;
    state = STIMULUS;

    if (currentStimulation >= flashingSequence->sequence.count())
    {
        currentLetter++;
        qDebug("STOPPED: isi Timer & Stim timer");
        isiTimer->stop();
        stimTimer->stop();

        //        if(currentLetter >= desired_phrase.length())
        //        {
        //            qDebug("Experiment End");

        //        }
        //        else
        //        {

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

void FlashingSpeller::pre_trial()
{
    qDebug()<< Q_FUNC_INFO;


    if (pre_trial_count == 0)
    {
        flashingSequence = new RandomFlashSequence(nr_elements, nr_sequence);
        sendMarker(OVTK_StimulationId_TrialStart);

        if (spelling_mode == CALIBRATION || spelling_mode == COPY_MODE)
        {
            text_row += desired_phrase[currentLetter];
            textRow->setText(text_row);
        }
    }

    preTrialTimer->start();
    pre_trial_count++;
    
    if (pre_trial_count > pre_trial_wait)
    {
        preTrialTimer->stop();
        pre_trial_count = 0;
        state = STIMULUS;
    }
}

void FlashingSpeller::feedback()
{
    qDebug() << Q_FUNC_INFO;

    if (spelling_mode == COPY_MODE){
        qDebug()<< "COPY MODE";
    }
    else {
        qDebug() << "FREE MODE";
    }

    post_trial();
}

void FlashingSpeller::post_trial()
{
    qDebug()<< Q_FUNC_INFO;

    sendMarker(OVTK_StimulationId_TrialStop);
    currentStimulation = 0;
    state = PRE_TRIAL;
    // wait
    //    wait(2000);

    if (currentLetter >= desired_phrase.length()){
        qDebug()<< "Experiment End";
        sendMarker(OVTK_StimulationId_ExperimentStop);
    }
    else{
        startTrial();
    }
}

/* Setters */
void FlashingSpeller::setSpeller_type(int value)
{
    speller_type = value;
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
