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
#include "ssvep.h"
#include "utils.h"
//
#include "flashingspeller.h"
#include "facespeller.h"
#include "erp.h"
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
    qDebug()<< "par "<<paradigm.stimulationType();
    qDebug()<< "brk "<<paradigm.breakDuration();
    if(noGui)
    {
        m_markerSender = new OVMarkerSender(this);
        if(!m_markerSender->Connect("127.0.0.1", "15361"))
        {
            qDebug()<< "Connection to OpenVibe acquisition server failed";
        }
        JsonSerializer jSerializer;
//        ERP paradigm;
        jSerializer.load(paradigm, configFile);
        qDebug()<< "brk "<<paradigm.breakDuration();
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
        launchTimer->setInterval(5000);
        launchTimer->setSingleShot(true);


        switch(spellerType)
        {
        case speller_type::FLASHING_SPELLER:
        {

            // JsonSerializer::save(ferp, fp, "");
            FlashingSpeller *flashSpeller = new FlashingSpeller();
            flashSpeller->initSpeller(paradigm);
            //            initSpeller(flashSpeller, spellerType, paradigm, launchTimer);
            connectSpeller(flashSpeller, launchTimer);
            break;
        }

        case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
        {
            FaceSpeller *faceSpeller = new FaceSpeller();
            faceSpeller->initSpeller(paradigm);
            //            initSpeller(faceSpeller, spellerType, paradigm, launchTimer);
            connectSpeller(faceSpeller, launchTimer);
            break;
        }
        }
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

//void ConfigPanel::initSpeller(Speller *t_sp, int t_spellerType, Paradigm prdg, QTimer *timer)
void ConfigPanel::connectSpeller(Speller *t_sp, QTimer *timer)
{

    connect(ui->startSpeller, SIGNAL(clicked()), t_sp, SLOT(startTrial()));
    connect(t_sp, SIGNAL(markerTag(uint64_t)), m_markerSender, SLOT(sendStimulation(uint64_t)));
    connect(timer, SIGNAL(timeout()), t_sp, SLOT(startTrial()));
    /*
    JsonSerializer jSerializer;

    if(noGui)
    {
        //        ERP ferp;
        jSerializer.load(prdg, configFile);
        t_sp->initSpeller(prdg);
        /*
        t_sp->setStimulationDuration(ferp.stimulationDuration());
        t_sp->setIsi(ferp.breakDuration());
        t_sp->setNrSequence(ferp.nrSequences());
        t_sp->setDesiredPhrase(ferp.desiredPhrase());
        t_sp->setSpellingMode(ferp.experimentMode());
        t_sp->setSpellerType(t_spellerType);
        t_sp->setFeedbackPort(12345);
        */
  /*  }
    else
    {
        ERP ferp(ui->spellingModeChoices->currentIndex(),
                 paradigm_type::ERP,
                 ui->stimulusDuration->text().toInt(),
                 ui->interStimulusDuration->text().toInt(),
                 ui->numberOfRepetition->text().toInt(),
                 ui->desiredPhrase->text(),
                 t_spellerType,
                 flashing_mode::SC);
        QString fp = "default.json";
        jSerializer.save(ferp, fp, "");
        t_sp->initSpeller(ferp);
        /*   t_sp->setStimulationDuration(ui->stimulusDuration->text().toInt());
        t_sp->setIsi(ui->interStimulusDuration->text().toInt());
        t_sp->setNrSequence(ui->numberOfRepetition->text().toInt());
        t_sp->setSpellingMode(ui->spellingModeChoices->currentIndex());
        t_sp->setDesiredPhrase(ui->desiredPhrase->text());
        t_sp->setSpellerType(t_spellerType);
        t_sp->setFeedbackPort(ui->feedback_port->text().toUShort());
        */
//    }


    qDebug()<< "[Config panel] [init speller] starting timer";
    timer->start();
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



