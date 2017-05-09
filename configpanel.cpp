#include "configpanel.h"
#include "ui_configpanel.h"
#include "mvepspeller.h"
#include "flashingspeller.h"
#include "ovmarkersender.h"
#include <QMessageBox>

const int FLASHING_SPELLER = 0;
const int FACES_SPELLER = 1;
const int MOTION_SPELLER = 2;

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
    int spellerType = ui->spellerType->currentIndex();


    switch(spellerType)
    {
    case FLASHING_SPELLER:
    {
        FlashingSpeller *Fspeller = new FlashingSpeller();

//        connect(ui->startSpeller, SIGNAL(clicked()), Fspeller, SLOT(startTrial()));
        connect(ui->startSpeller, SIGNAL(clicked()), Fspeller, SLOT(tick()) );
        connect(Fspeller, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));

        Fspeller->setStimulation_duration(ui->stimulusDuration->text().toInt());
        Fspeller->setIsi(ui->interStimulusDuration->text().toInt());
        Fspeller->setNr_sequence(ui->numberOfRepetition->text().toInt());
        Fspeller->setSpelling_mode(ui->spellingModeChoices->currentIndex());
        Fspeller->setDesired_phrase(ui->desiredPhrase->text());
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
        Fspeller->setSpeller_type(ui->spellerType->currentIndex());

        break;
    }


    case MOTION_SPELLER:
    {
        mVEPSpeller *Mspeller = new mVEPSpeller();
        break;
    }
    }
    //    speller = new mVEPSpeller();

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
