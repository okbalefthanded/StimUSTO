#include "configpanel.h"
#include "ui_configpanel.h"
#include "mvepspeller.h"
#include "flashingspeller.h"
#include "movingface.h"
#include "ovmarkersender.h"
#include <QMessageBox>

const quint8 FLASHING_SPELLER = 0;
const quint8 FACES_SPELLER = 1;
const quint8 MOTION_BAR = 2;
const quint8 MOTION_FACE = 3;
const quint8 MOVING_FACE = 4;
const quint8 SSVEP = 5;

ConfigPanel::ConfigPanel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigPanel)
{
    ui->setupUi(this);
}

ConfigPanel::~ConfigPanel()
{
    delete ui;

}
//set a TCP socket connection to OpenVibe Acquisition Client
/**
 * @brief ConfigPanel::on_connectOvAsBtn_clicked
 */
void ConfigPanel::on_connectOvAsBtn_clicked()
{
    QString ovAsAddress = ui->addressOvAs->text();
    QString ovTcpTagPort = ui->portOvAs->text();
    cTest = new OVMarkerSender(this);

    if (cTest->Connect(ovAsAddress,ovTcpTagPort))

    {
        QMessageBox::information(this,"Socket connection","Connected");

    }

    else
    {

        QMessageBox::information(this,"Socket connection","Not Connected");
    }
}

//TODO
// init speller
/**
 * @brief ConfigPanel::on_initSpeller_clicked
 */
void ConfigPanel::on_initSpeller_clicked()
{
    if(!cTest->connectedOnce)
    {
        QMessageBox::information(this,"Socket connection","Not Connected");
    }
    else {

        int spellerType = ui->spellerType->currentIndex();

        switch(spellerType)
        {
        case FLASHING_SPELLER:
        {
            FlashingSpeller *Fspeller = new FlashingSpeller();

            connect(ui->startSpeller, SIGNAL(clicked()), Fspeller, SLOT(startTrial()));
            connect(Fspeller, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));

            Fspeller->setStimulation_duration(ui->stimulusDuration->text().toInt());
            Fspeller->setIsi(ui->interStimulusDuration->text().toInt());
            Fspeller->setNr_sequence(ui->numberOfRepetition->text().toInt());
            Fspeller->setSpelling_mode(ui->spellingModeChoices->currentIndex());
            Fspeller->setDesired_phrase(ui->desiredPhrase->text());
            Fspeller->setSpeller_type(spellerType);
            Fspeller->setFeedbackPort(ui->feedback_port->text().toInt());
            break;
        }
        case FACES_SPELLER:
        {
            //TODO

            FlashingSpeller *Fspeller = new FlashingSpeller();
            //
            connect(ui->startSpeller, SIGNAL(clicked()), Fspeller, SLOT(startTrial()));
            connect(Fspeller, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));
            //
            Fspeller->setStimulation_duration(ui->stimulusDuration->text().toInt());
            Fspeller->setIsi(ui->interStimulusDuration->text().toInt());
            Fspeller->setNr_sequence(ui->numberOfRepetition->text().toInt());
            Fspeller->setSpelling_mode(ui->spellingModeChoices->currentIndex());
            Fspeller->setDesired_phrase(ui->desiredPhrase->text());
            Fspeller->setSpeller_type(spellerType);
            Fspeller->setFeedbackPort(ui->feedback_port->text().toInt());
            break;
        }

        case MOTION_BAR:
        case MOTION_FACE:
        {
            mVEPSpeller *Mspeller = new mVEPSpeller(spellerType);
            connect(ui->startSpeller, SIGNAL(clicked()), Mspeller, SLOT(startTrial()));
            connect(Mspeller, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));

            Mspeller->setStimulation_duration(ui->stimulusDuration->text().toInt());
            Mspeller->setIsi(ui->interStimulusDuration->text().toInt());
            Mspeller->setNr_sequence(ui->numberOfRepetition->text().toInt());
            Mspeller->setSpelling_mode(ui->spellingModeChoices->currentIndex());
            Mspeller->setDesired_phrase(ui->desiredPhrase->text());
            Mspeller->setSpeller_type(spellerType);
            Mspeller->setFeedbackPort(ui->feedback_port->text().toInt());
            Mspeller->setAnimTimeUpdateIntervale(ui->stimulusDuration->text().toInt());
            break;
        }

        case MOVING_FACE:
        {
            MovingFace *movingFaceSpeller = new MovingFace();
            connect(ui->startSpeller, SIGNAL(clicked()), movingFaceSpeller, SLOT(startTrial()));
            connect(movingFaceSpeller, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));

            movingFaceSpeller->setStimulation_duration(ui->stimulusDuration->text().toInt());
            movingFaceSpeller->setIsi(ui->interStimulusDuration->text().toInt());
            movingFaceSpeller->setNr_sequence(ui->numberOfRepetition->text().toInt());
            movingFaceSpeller->setSpelling_mode(ui->spellingModeChoices->currentIndex());
            movingFaceSpeller->setDesired_phrase(ui->desiredPhrase->text());
            movingFaceSpeller->setSpeller_type(spellerType);
            movingFaceSpeller->setFeedbackPort(ui->feedback_port->text().toInt());

            break;
        }
        case SSVEP:
            break;
        }
    }
}

//TODO
//Start speller
void ConfigPanel::on_startSpeller_clicked()
{
    this->onStart = true;
}

//TODO
//Pause speller
void ConfigPanel::on_pauseSpeller_clicked()
{

}

//TODO
//Stop speller
void ConfigPanel::on_stopSpeller_clicked()
{

}

//TODO
//Quit speller
void ConfigPanel::on_quitSpeller_clicked()
{

}
