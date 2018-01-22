//
#include <QMessageBox>

//
#include "configpanel.h"
#include "ui_configpanel.h"
#include "mvepspeller.h"
#include "flashingspeller.h"
#include "movingface.h"
#include "ovmarkersender.h"
#include "ssvep.h"
#include "coloredface.h"
#include "hybridstimulation.h"
#include "hybridgridstimulation.h"
// speller types
const quint8 FLASHING_SPELLER = 0;
const quint8 FACES_SPELLER = 1;
const quint8 MOTION_BAR = 2;
const quint8 MOTION_FACE = 3;
const quint8 MOVING_FACE = 4;
const quint8 SSVEP = 5;
const quint8 INVERTED_FACE = 6;
const quint8 COLORED_FACE = 7;
const quint8 INVERTED_COLORED_FACE = 8;
// operation modes
const quint8 CALIBRATION = 0;
const quint8 COPY_MODE  = 1;
const quint8 FREE_MODE = 2;
const quint8 SSVEP_SINGLE = 3;
const int start_port = 54321;

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

    QHostAddress sender;
    quint16 senderPort = 54321;
    QByteArray *buffer = new QByteArray();

    //    buffer->resize(start_socket->pendingDatagramSize());
    //    //    qDebug() << "buffer size" << buffer->size();

    //    start_socket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);

    start_socket = new QUdpSocket(this);
    start_socket->bind(QHostAddress("10.3.65.37"), 54321);

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
        case INVERTED_FACE:
        {
            FlashingSpeller *Fspeller = new FlashingSpeller();
            //
            connect(ui->startSpeller, SIGNAL(clicked()), Fspeller, SLOT(startTrial()));
            connect(start_socket, SIGNAL(readyRead()), Fspeller, SLOT(startTrial()));
            connect(Fspeller, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));
            //
            Fspeller->setStimulation_duration(ui->stimulusDuration->text().toInt());
            Fspeller->setIsi(ui->interStimulusDuration->text().toInt());
            Fspeller->setNr_sequence(ui->numberOfRepetition->text().toInt());
            Fspeller->setSpelling_mode(ui->spellingModeChoices->currentIndex());
            Fspeller->setDesired_phrase(ui->desiredPhrase->text());
            Fspeller->setSpeller_type(spellerType);
            Fspeller->setFeedbackPort(ui->feedback_port->text().toInt());
            //            buffer->resize(start_socket->pendingDatagramSize());
            //            qDebug()<< "Start Data" << buffer->data();
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
        case COLORED_FACE:
        case INVERTED_COLORED_FACE:
        {
            ColoredFace *coloredFaceSpeller = new ColoredFace();
            connect(ui->startSpeller, SIGNAL(clicked()), coloredFaceSpeller, SLOT(startTrial()));
            connect(coloredFaceSpeller, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));

            coloredFaceSpeller->setStimulation_duration(ui->stimulusDuration->text().toInt());
            coloredFaceSpeller->setIsi(ui->interStimulusDuration->text().toInt());
            coloredFaceSpeller->setNr_sequence(ui->numberOfRepetition->text().toInt());
            coloredFaceSpeller->setSpelling_mode(ui->spellingModeChoices->currentIndex());
            coloredFaceSpeller->setDesired_phrase(ui->desiredPhrase->text());
            coloredFaceSpeller->setSpeller_type(spellerType);
            coloredFaceSpeller->setFeedbackPort(ui->feedback_port->text().toInt());
            break;
        }
        }
    }
}

//TODO
//INIT SSVEP
void ConfigPanel::on_initSSVEP_clicked()
{
    qDebug()<<Q_FUNC_INFO;


    if(!cTest->connectedOnce)
    {
        QMessageBox::information(this,"Socket connection","Not Connected");
    }

    else
    {
        quint8 SSVEPNrElements;
        quint8 operationMode;
        if (ui->SSVEP_mode->currentIndex() == SSVEP_SINGLE)
        {
            SSVEPNrElements = 1;
            operationMode = CALIBRATION;
        }
        else
        {
            QStringList freqsList = ui->Frequencies->text().split(',');
            SSVEPNrElements = freqsList.count() + 1;
            operationMode = ui->SSVEP_mode->currentIndex();
        }

        Ssvep *ssvepStimulation = new Ssvep(SSVEPNrElements);

        connect(ui->startSpeller, SIGNAL(clicked()), ssvepStimulation, SLOT(startTrial()));
        connect(ssvepStimulation, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));

        ssvepStimulation->setFrequencies(ui->Frequencies->text());
        ssvepStimulation->setStimulationDuration(ui->SSVEP_StimDuration->text().toFloat());
        ssvepStimulation->setBreakDuration(ui->SSVEP_BreakDuration->text().toInt());
        ssvepStimulation->setSequence(ui->SSVEP_Sequence->text().toInt());
        ssvepStimulation->setFlickeringMode(operationMode);
        ssvepStimulation->setFeedbackPort(ui->feedback_port->text().toInt());


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

void ConfigPanel::on_initHybrid_clicked()
{

    int spellerType = ui->spellerType->currentIndex();

    if(!cTest->connectedOnce)
    {
        QMessageBox::information(this,"Socket connection","Not Connected");
    }
    else
    {
        //        HybridStimulation *hybrid = new HybridStimulation();

        HybridGridStimulation *hybrid = new HybridGridStimulation();
        connect(ui->startSpeller, SIGNAL(clicked()), hybrid, SLOT(startTrial()));
        connect(hybrid, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));
        //ERP configuration
        hybrid->setERPStimulationDuration(ui->stimulusDuration->text().toInt());
        hybrid->setIsi(ui->interStimulusDuration->text().toInt());
        hybrid->setERPNrSequence(ui->numberOfRepetition->text().toInt());
        hybrid->setOperationMode(ui->spellingModeChoices->currentIndex());
        hybrid->setDesiredSequence(ui->desiredPhrase->text());
        hybrid->setERPStimulationType(spellerType);
        //SSVEP configuration
        hybrid->setFrequencies(ui->Frequencies->text());
        hybrid->setSSVEPStimulationDuration(ui->SSVEP_StimDuration->text().toInt());
        //        hybrid->setBreakDuration(ui->SSVEP_BreakDuration->text().toInt());
        //        hybrid->setSSVEPSequence(ui->SSVEP_Sequence->text().toInt());
        //        hybrid->setFlickeringMode(ui->SSVEP_mode->currentIndex());
        // general configuration
        hybrid->setFeedbackPort(ui->feedback_port->text().toInt());
    }
}



