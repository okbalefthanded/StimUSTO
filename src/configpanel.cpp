//
#include <QMessageBox>
#include <QTimer>
#include <serializable.h>
//
#include "configpanel.h"
#include "ui_configpanel.h"
#include "ovmarkersender.h"
#include "speller.h"
// #include "hybridelipsestimulation.h"
// #include "hybridgridstimulation.h"
#include "ssvepgl.h"
#include "hybridstimulation.h"
#include "utils.h"
#include "paradigm.h"
//
#include "flashingspeller.h"
#include "facespeller.h"
#include "erp.h"
#include "ssvep.h"
#include "hybrid.h"
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
    else if(paradigmType == paradigm_type::HYBRID)
    {

        on_initHybrid_clicked();
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

    // m_startSocket = new QUdpSocket(this);
    // m_startSocket->bind(QHostAddress("10.3.65.37"), 54321);

    int spellerType = 0;

    ERP *erpParadigm = new ERP();

    if(noGui)
    {
        initParadigmJSon(erpParadigm);
    }

    else
    {
        erpParadigm = initParadigmERPGui();
    }

    spellerType = erpParadigm->stimulationType();

    if(!m_markerSender->connectedOnce())
    {
        QMessageBox::information(this, "Socket connection", "Not Connected");
    }
    else
    {
        QTimer *launchTimer = new QTimer();
        launchTimer->setInterval(10000);
        launchTimer->setSingleShot(true);

        Speller *speller = createSpeller(spellerType);
        speller->initSpeller(erpParadigm);
        connectParadigm(speller, launchTimer);


    }
}


//INIT SSVEP
void ConfigPanel::on_initSSVEP_clicked()
{

    SSVEP *ssvepParadigm = new SSVEP();

    if(noGui)
    {
        initParadigmJSon(ssvepParadigm);
    }
    else
    {
        ssvepParadigm = initParadigmSSVEPGui();
    }

    if(!m_markerSender->connectedOnce())
    {
        QMessageBox::information(this,"Socket connection","Not Connected");
    }

    else
    {
        //
        QTimer *launchTimer = new QTimer();
        launchTimer->setInterval(10000);
        launchTimer->setSingleShot(true);

        SsvepGL *ssvepStimulation = createSSVEP(ssvepParadigm);
        connectParadigm(ssvepStimulation, launchTimer);
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
    Hybrid *hybridParadigm = new Hybrid();

    if(noGui)
    {
        initParadigmJSon(hybridParadigm);
    }

    else
    {
        ERP *erpParadigm = initParadigmERPGui();
        SSVEP *ssvepParadigm = initParadigmSSVEPGui();
        *hybridParadigm = Hybrid(erpParadigm, ssvepParadigm);

    }
    if(!m_markerSender->connectedOnce())
    {
        QMessageBox::information(this,"Socket connection","Not Connected");
    }
    else
    {
        QTimer *launchTimer = new QTimer();
        launchTimer->setInterval(10000);
        launchTimer->setSingleShot(true);

        // HybridStimulation *hybrid = new HybridStimulation(hybridParadigm);
        Speller *speller = createSpeller(hybridParadigm->m_ERPparadigm->type());
        speller->initSpeller(hybridParadigm->m_ERPparadigm);
        SsvepGL *ssvepStimulation = createSSVEP(hybridParadigm->m_SSVEPparadigm);
        HybridStimulation *hybrid = new HybridStimulation(hybridParadigm, speller, ssvepStimulation);
        connectParadigm(hybrid, launchTimer);

        //        HybridStimulation *hybrid = new HybridStimulation();
        //        HybridGridStimulation *hybrid = new HybridGridStimulation();
        //        connect(ui->startSpeller, SIGNAL(clicked()), hybrid, SLOT(startTrial()));
        //        connect(hybrid, SIGNAL(markerTag(uint64_t)), m_markerSender, SLOT(sendStimulation(uint64_t)));
        //ERP configuration
        //        hybrid->setERPStimulationDuration(ui->stimulusDuration->text().toInt());
        //        hybrid->setIsi(ui->interStimulusDuration->text().toInt());
        //        hybrid->setERPNrSequence(ui->numberOfRepetition->text().toInt());
        //        hybrid->setOperationMode(ui->spellingModeChoices->currentIndex());
        //        hybrid->setDesiredSequence(ui->desiredPhrase->text());
        //        hybrid->setERPStimulationType(spellerType);
        //SSVEP configuration
        // hybrid->setFrequencies(ui->Frequencies->text());
        // hybrid->setSSVEPStimulationDuration(ui->SSVEP_StimDuration->text().toInt());
        //        hybrid->setBreakDuration(ui->SSVEP_BreakDuration->text().toInt());
        //        hybrid->setSSVEPSequence(ui->SSVEP_Sequence->text().toInt());
        //        hybrid->setFlickeringMode(ui->SSVEP_mode->currentIndex());
        // general configuration
        // hybrid->setFeedbackPort(ui->feedback_port->text().toUShort());
    }
}

ERP *ConfigPanel::initParadigmERPGui()
{
    ERP *erpParadigm = new ERP();
    int spellerType = ui->spellerType->currentIndex();
    erpParadigm = new ERP(ui->spellingModeChoices->currentIndex(),
                          control_mode::SYNC, // TODO : implement async ERP control mode
                          paradigm_type::ERP,
                          ui->stimulusDuration->text().toInt(),
                          ui->interStimulusDuration->text().toInt(),
                          ui->numberOfRepetition->text().toInt(),
                          ui->desiredPhrase->text(),
                          spellerType,
                          flashing_mode::SC);
    return erpParadigm;
}

Speller *ConfigPanel::createSpeller(int t_spellerType)
{
    switch(t_spellerType)
    {
    case speller_type::FLASHING_SPELLER:
    {
        FlashingSpeller *flashSpeller = new FlashingSpeller();
        connectStimulation(flashSpeller);
        /* flashSpeller->initSpeller(erpParadigm);
        connectParadigm(flashSpeller, launchTimer);*/
        return flashSpeller;
        // break;
    }

        // case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
    case speller_type::FACES_SPELLER:
    case speller_type::INVERTED_FACE:
    case speller_type::COLORED_FACE:
    case speller_type::INVERTED_COLORED_FACE:
    {
        FaceSpeller *faceSpeller = new FaceSpeller();
        connectStimulation(faceSpeller);
        qDebug()<< Q_FUNC_INFO << "returning a FaceSpeller";
        return  faceSpeller;
        //  faceSpeller->initSpeller(erpParadigm);
        //  connectParadigm(faceSpeller, launchTimer);
        //  break;
    }
    }
    //return speller;
}

SSVEP *ConfigPanel::initParadigmSSVEPGui()
{
    SSVEP *ssvepParadigm = new SSVEP();

    int SSVEPNrElements;
    int operationMode;

    QStringList freqsList = ui->Frequencies->text().split(',');
    operationMode = ui->SSVEP_Mode->currentIndex();

    if (operationMode == operation_mode::SSVEP_SINGLE)
    {
        SSVEPNrElements = 1;
        operationMode = operation_mode::CALIBRATION;
    }
    else if(ui->SSVEP_Control->currentIndex() == control_mode::SYNC)
    {
        SSVEPNrElements = freqsList.count();
    }
    else
    {
        SSVEPNrElements = freqsList.count() + 1;
    }

    ssvepParadigm = new SSVEP(operationMode,
                              ui->SSVEP_Control->currentIndex(),
                              paradigm_type::SSVEP,
                              ui->SSVEP_StimDuration->text().toFloat(),
                              ui->SSVEP_BreakDuration->text().toFloat(),
                              ui->SSVEP_Sequence->text().toInt(),
                              "",
                              SSVEPNrElements,
                              ui->Frequencies->text(),
                              ui->SSVEP_Stimulation->currentIndex());
    return ssvepParadigm;
}

SsvepGL *ConfigPanel::createSSVEP(SSVEP *t_ssvep)
{

    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    // format.setVersion(3,3);
    format.setVersion(3,0); // ANGLE supports ES 3.0, higher versions raise exceptions

    SsvepGL *ssvepStimulation = new SsvepGL(*t_ssvep);
    ssvepStimulation->setFormat(format);

    if(QGuiApplication::screens().size() == 2)
    {
        ssvepStimulation->resize(utils::getScreenSize());
    }
    else
    {
        ssvepStimulation->resize(QSize(1366, 768)); //temporaty size;
    }

    connectStimulation(ssvepStimulation);

    return ssvepStimulation;

}

void ConfigPanel::initParadigmJSon(Paradigm *prdg)
{

    m_markerSender = new OVMarkerSender(this);
    if(!m_markerSender->Connect("127.0.0.1", "15361"))
    {
        qDebug()<< "Connection to OpenVibe acquisition server failed";
    }

    JsonSerializer jSerializer;
    jSerializer.load(*prdg, configFile);
}

void ConfigPanel::connectStimulation(QObject *t_obj)
{
    connect(t_obj, SIGNAL(markerTag(uint64_t)), m_markerSender, SLOT(sendStimulation(uint64_t)));

}

void ConfigPanel::connectParadigm(QObject *pr, QTimer *timer)
{


    if(noGui)
    {
        connect(timer, SIGNAL(timeout()), pr, SLOT(startTrial()));
        timer->start();
    }
    else
    {
        connect(ui->startSpeller, SIGNAL(clicked()), pr, SLOT(startTrial()));
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



