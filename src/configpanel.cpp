//
#include <QMessageBox>
#include <QTimer>
//
#include "configpanel.h"
#include "ui_configpanel.h"
#include "ovmarkersender.h"
#include "speller.h"
#include "hybridstimulation.h"
#include "hybridgridstimulation.h"
#include "ssvepgl.h"
//#include "ssvep_timer.h"
#include "utils.h"
#include "paradigm.h"
//
#include "flashingspeller.h"
#include "facespeller.h"
#include "erp.h"
#include "ssvep.h"
#include "jsonserializer.h"
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

void ConfigPanel::startExperiment()
{
    JsonSerializer jSerializer;
    Paradigm paradigm;
    jSerializer.load(paradigm, configFile);
    quint8  paradigmType = paradigm.type();

    if(paradigmType == paradigm_type::ERP)
    {
        on_initSpeller_clicked();
    }
    else if(paradigmType == paradigm_type::SSVEP)
    {
        on_initSSVEP_clicked();
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


    int spellerType = 0;
    ERP paradigm;

    if(noGui)
    {
        m_markerSender = new OVMarkerSender(this);
        if(!m_markerSender->Connect("127.0.0.1", "15361"))
        {
            qDebug()<< "Connection to OpenVibe acquisition server failed";
        }
        JsonSerializer jSerializer;
        jSerializer.load(paradigm, configFile);
        spellerType = paradigm.stimulationType();
    }

    else
    {
        spellerType = ui->spellerType->currentIndex();
        ERP paradigm(ui->spellingModeChoices->currentIndex(),
                     paradigm_type::ERP,
                     ui->stimulusDuration->text().toInt(),
                     ui->interStimulusDuration->text().toInt(),
                     ui->numberOfRepetition->text().toInt(),
                     ui->desiredPhrase->text(),
                     spellerType,
                     flashing_mode::SC);

    }

    if(!m_markerSender->connectedOnce())
    {
        QMessageBox::information(this, "Socket connection", "Not Connected");
    }
    else
    {
        QTimer *launchTimer = new QTimer();
        launchTimer->setInterval(10000);
        launchTimer->setSingleShot(true);


        switch(spellerType)
        {
        case speller_type::FLASHING_SPELLER:
        {

            FlashingSpeller *flashSpeller = new FlashingSpeller();
            flashSpeller->initSpeller(paradigm);
            connectSpeller(flashSpeller, launchTimer);
            break;
        }

        case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
        {
            FaceSpeller *faceSpeller = new FaceSpeller();
            faceSpeller->initSpeller(paradigm);
            connectSpeller(faceSpeller, launchTimer);
            break;
        }
        }
    }
}


//INIT SSVEP
void ConfigPanel::on_initSSVEP_clicked()
{

    SSVEP *ssvepParadigm = new SSVEP();

    if(noGui)
    {
        m_markerSender = new OVMarkerSender(this);
        if(!m_markerSender->Connect("127.0.0.1", "15361"))
        {
            qDebug()<< "Connection to OpenVibe acquisition server failed";
        }

        JsonSerializer jSerializer;
        jSerializer.load(*ssvepParadigm, configFile);

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

        ssvepParadigm = new SSVEP(operationMode,
                                  paradigm_type::SSVEP,
                                  ui->SSVEP_StimDuration->text().toFloat(),
                                  ui->SSVEP_BreakDuration->text().toFloat(),
                                  ui->SSVEP_Sequence->text().toInt(),
                                  "",
                                  SSVEPNrElements,
                                  ui->Frequencies->text());

    }
    if(!m_markerSender->connectedOnce())
    {
        QMessageBox::information(this,"Socket connection","Not Connected");
    }

    else
    {
        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setVersion(3,3);

        SsvepGL *ssvepStimulation = new SsvepGL(*ssvepParadigm);
        ssvepStimulation->setFormat(format);
        // ssvepStimulation->resize(QSize(1000, 700));//temporaty size;
        ssvepStimulation->resize(utils::getScreenSize());
        //
        QTimer *launchTimer = new QTimer();
        launchTimer->setInterval(10000);
        launchTimer->setSingleShot(true);

        connectSSVEP(ssvepStimulation, launchTimer);

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

void ConfigPanel::connectSpeller(Speller *t_sp, QTimer *timer)
{
    connect(ui->startSpeller, SIGNAL(clicked()), t_sp, SLOT(startTrial()));
    connect(t_sp, SIGNAL(markerTag(uint64_t)), m_markerSender, SLOT(sendStimulation(uint64_t)));
    connect(timer, SIGNAL(timeout()), t_sp, SLOT(startTrial()));

    timer->start();
}

void ConfigPanel::connectSSVEP(SsvepGL *ssvep, QTimer *timer)
{
    connect(ssvep, SIGNAL(markerTag(uint64_t)), m_markerSender, SLOT(sendStimulation(uint64_t)));

    if(noGui)
    {
        connect(timer, SIGNAL(timeout()), ssvep, SLOT(startTrial()));
        timer->start();
    }
    else
    {
        connect(ui->startSpeller, SIGNAL(clicked()), ssvep, SLOT(startTrial()));
    }
}

bool ConfigPanel::getNoGui() const
{
    return noGui;
}

void ConfigPanel::setNoGui(bool value)
{
    noGui = value;
}

QString ConfigPanel::getConfigFile() const
{
    return configFile;
}

void ConfigPanel::setConfigFile(const QString &value)
{
    configFile = value;
}

ConfigPanel::~ConfigPanel()
{
    delete ui;

}



