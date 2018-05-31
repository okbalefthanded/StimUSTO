//
#include <QMessageBox>
//
#include "configpanel.h"
#include "ui_configpanel.h"
#include "test.h"
#include "mvepspeller.h"
#include "flashingspeller.h"
#include "movingface.h"
#include "ovmarkersender.h"
#include "ssvep.h"
#include "coloredface.h"
#include "hybridstimulation.h"
#include "hybridgridstimulation.h"
#include "ssvepgl.h"
#include "utils.h"
//

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
    else
    {
        int spellerType = ui->spellerType->currentIndex();
        Speller *Fspeller = new Speller();

        connect(ui->startSpeller, SIGNAL(clicked()), Fspeller, SLOT(startTrial()));
        connect(Fspeller, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));

        Fspeller->setStimulationDuration(ui->stimulusDuration->text().toInt());
        Fspeller->setIsi(ui->interStimulusDuration->text().toInt());
        Fspeller->setNrSequence(ui->numberOfRepetition->text().toInt());
        Fspeller->setSpellingMode(ui->spellingModeChoices->currentIndex());
        Fspeller->setDesiredPhrase(ui->desiredPhrase->text());
        Fspeller->setSpellerType(spellerType);
        Fspeller->setFeedbackPort(ui->feedback_port->text().toInt());
    }
}


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
        if (ui->SSVEP_mode->currentIndex() == operation_mode::SSVEP_SINGLE)
        {
            SSVEPNrElements = 1;
            operationMode = operation_mode::CALIBRATION;
        }
        else
        {
            QStringList freqsList = ui->Frequencies->text().split(',');
            SSVEPNrElements = freqsList.count() + 1;
            operationMode = ui->SSVEP_mode->currentIndex();
        }

        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setVersion(3,3);

        SsvepGL *ssvepStimulation = new SsvepGL(SSVEPNrElements);

        ssvepStimulation->setFormat(format);
        // ssvepStimulation->resize(QSize(1000, 700));//temporaty size;
        ssvepStimulation->resize(utils::getScreenSize());
        //
        connect(ui->startSpeller, SIGNAL(clicked()), ssvepStimulation, SLOT(startTrial()));
        connect(ssvepStimulation, SIGNAL(markerTag(uint64_t)), cTest, SLOT(sendStimulation(uint64_t)));

        ssvepStimulation->setFrequencies(ui->Frequencies->text());
        ssvepStimulation->setStimulationDuration(ui->SSVEP_StimDuration->text().toFloat());
        ssvepStimulation->setBreakDuration(ui->SSVEP_BreakDuration->text().toInt());
        ssvepStimulation->setSequence(ui->SSVEP_Sequence->text().toInt());
        ssvepStimulation->setFlickeringMode(operationMode);
        ssvepStimulation->setFeedbackPort(ui->feedback_port->text().toInt());
        //
        ssvepStimulation->show();
    }
}

//TODO
//Start speller
void ConfigPanel::on_startSpeller_clicked()
{
    onStart = true;
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



