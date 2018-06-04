//
#include <QMessageBox>
//
#include "configpanel.h"
#include "ui_configpanel.h"
#include "ovmarkersender.h"
#include "speller.h"
#include "hybridstimulation.h"
#include "hybridgridstimulation.h"
#include "ssvepgl.h"
#include "ssvep.h"
#include "utils.h"
//

ConfigPanel::ConfigPanel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigPanel)
{
    ui->setupUi(this);
}


//set a TCP socket connection to OpenVibe Acquisition Client
/**
 * @brief ConfigPanel::on_connectOvAsBtn_clicked
 */
void ConfigPanel::on_connectOvAsBtn_clicked()
{
    QString ovAsAddress = ui->addressOvAs->text();
    QString ovTcpTagPort = ui->portOvAs->text();
    m_markerSender = new OVMarkerSender(this);

    if (m_markerSender->Connect(ovAsAddress, ovTcpTagPort))
    {
        QMessageBox::information(this, "Socket connection", "Connected");
    }
    else
    {
        QMessageBox::information(this, "Socket connection", "Not Connected");
    }
}


// init speller
/**
 * @brief ConfigPanel::on_initSpeller_clicked
 */
void ConfigPanel::on_initSpeller_clicked()
{

    // QHostAddress sender;
    // quint16 senderPort = 54321;
    // QByteArray *buffer = new QByteArray();
    //    buffer->resize(start_socket->pendingDatagramSize());
    //    qDebug() << "buffer size" << buffer->size();
    //    start_socket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);

    m_startSocket = new QUdpSocket(this);
    m_startSocket->bind(QHostAddress("10.3.65.37"), 54321);

    if(!m_markerSender->connectedOnce())
    {
        QMessageBox::information(this, "Socket connection", "Not Connected");
    }
    else
    {
        int spellerType = ui->spellerType->currentIndex();
        Speller *fSpeller = new Speller();

        connect(ui->startSpeller, SIGNAL(clicked()), fSpeller, SLOT(startTrial()));
        connect(fSpeller, SIGNAL(markerTag(uint64_t)), m_markerSender, SLOT(sendStimulation(uint64_t)));

        fSpeller->setStimulationDuration(ui->stimulusDuration->text().toInt());
        fSpeller->setIsi(ui->interStimulusDuration->text().toInt());
        fSpeller->setNrSequence(ui->numberOfRepetition->text().toInt());
        fSpeller->setSpellingMode(ui->spellingModeChoices->currentIndex());
        fSpeller->setDesiredPhrase(ui->desiredPhrase->text());
        fSpeller->setSpellerType(spellerType);
        fSpeller->setFeedbackPort(ui->feedback_port->text().toUShort());
    }
}


//INIT SSVEP
void ConfigPanel::on_initSSVEP_clicked()
{
    qDebug()<<Q_FUNC_INFO;


    if(!m_markerSender->connectedOnce())
    {
        QMessageBox::information(this,"Socket connection","Not Connected");
    }

    else
    {
        int SSVEPNrElements;
        int operationMode;

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
        connect(ssvepStimulation, SIGNAL(markerTag(uint64_t)), m_markerSender, SLOT(sendStimulation(uint64_t)));

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
    m_onStart = true;
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

    if(!m_markerSender->connectedOnce())
    {
        QMessageBox::information(this,"Socket connection","Not Connected");
    }
    else
    {
        //        HybridStimulation *hybrid = new HybridStimulation();

        HybridGridStimulation *hybrid = new HybridGridStimulation();
        connect(ui->startSpeller, SIGNAL(clicked()), hybrid, SLOT(startTrial()));
        connect(hybrid, SIGNAL(markerTag(uint64_t)), m_markerSender, SLOT(sendStimulation(uint64_t)));
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
        hybrid->setFeedbackPort(ui->feedback_port->text().toUShort());
    }
}

ConfigPanel::~ConfigPanel()
{
    delete ui;

}



