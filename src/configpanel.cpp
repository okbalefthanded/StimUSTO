//
#include <QMessageBox>
#include <QTimer>
#include <QGuiApplication>
#include <serializable.h>
//
#include "configpanel.h"
#include "ui_configpanel.h"
#include "ovmarkersender.h"
#include "speller.h"
#include "ssvepgl.h"
#include "hybridstimulation.h"
#include "utils.h"
#include "paradigm.h"
//
#include "flashingspeller.h"
#include "facespeller.h"
#include "arabicspeller.h"
#include "multistimuli.h"
#include "erp.h"
#include "ssvep.h"
#include "hybrid.h"
#include "jsonserializer.h"
#include "ovtk_stimulations.h"
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
        speller->setERP(erpParadigm);
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

        SsvepGL *ssvepStimulation = createSSVEP(ssvepParadigm, 12345);
        connectParadigm(ssvepStimulation, launchTimer);
        // ssvepStimulation->show();
        ssvepStimulation->showFullScreen();

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

//Quit speller
void ConfigPanel::on_quitSpeller_clicked()
{
    m_markerSender->sendStimulation(OVTK_StimulationId_ExperimentStop);

    if(noGui)
    {
        QGuiApplication::quit();
    }
    else
    {
        this->close();
    }
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

        Speller *speller = createSpeller(hybridParadigm->m_ERPparadigm->stimulationType());
        speller->setERP(hybridParadigm->m_ERPparadigm);
        speller->setPresentFeedback(false);
        SsvepGL *ssvepStimulation = createSSVEP(hybridParadigm->m_SSVEPparadigm, 12346);
        ssvepStimulation->setPresentFeedback(false);

        HybridStimulation *hybrid = new HybridStimulation(hybridParadigm, speller, ssvepStimulation);
        connectParadigm(hybrid, launchTimer);
        connect(hybrid, SIGNAL(experimentEnd()), this, SLOT(on_quitSpeller_clicked()));
    }
}

ERP *ConfigPanel::initParadigmERPGui()
{
    ERP *erpParadigm = new ERP();
    int spellerType = ui->spellerType->currentIndex();
    erpParadigm = new ERP(ui->spellingModeChoices->currentIndex(),
                          control_mode::SYNC, // TODO : implement async ERP control mode
                          paradigm_type::ERP,
                          external_comm::DISABLED,
                          ui->stimulusDuration->text().toInt(),
                          ui->interStimulusDuration->text().toInt(),
                          ui->numberOfRepetition->text().toInt(),
                          ui->desiredPhrase->text(),
                          "127.0.0.1",
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
        return flashSpeller;
    }

        // case speller_type::FACES_SPELLER ... speller_type::INVERTED_COLORED_FACE: //mingw/gcc only
    case speller_type::FACES_SPELLER:
    case speller_type::INVERTED_FACE:
    case speller_type::COLORED_FACE:
    case speller_type::INVERTED_COLORED_FACE:
    {
        FaceSpeller *faceSpeller = new FaceSpeller();
        connectStimulation(faceSpeller);
        return  faceSpeller;
    }

    case speller_type::ARABIC_SPELLER:
    {
        ArabicSpeller *arabicSpeller = new ArabicSpeller();
        connectStimulation(arabicSpeller);
        return arabicSpeller;

    }

    case speller_type::DUAL_STIM:
    case speller_type::MULTI_STIM:
    {
        MultiStimuli *multiStimuli = new MultiStimuli();
        connectStimulation(multiStimuli);
        return multiStimuli;
    }

    }

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
                              external_comm::DISABLED,
                              ui->SSVEP_StimDuration->text().toFloat(),
                              ui->SSVEP_BreakDuration->text().toFloat(),
                              ui->SSVEP_Sequence->text().toInt(),
                              "",
                              "127.0.0.1",
                              SSVEPNrElements,
                              ui->Frequencies->text(),
                              ui->SSVEP_Stimulation->currentIndex());
    return ssvepParadigm;
}

SsvepGL *ConfigPanel::createSSVEP(SSVEP *t_ssvep, int t_port)
{

    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapInterval(1); // vsync on
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer); //
    // format.setVersion(3,0);
    format.setVersion(3 ,0); // ANGLE supports ES 3.0, higher versions raise exceptions
    // format.setVersion(4, 5); // HP ProBook

    SsvepGL *ssvepStimulation = new SsvepGL(t_ssvep, t_port);
    ssvepStimulation->setFormat(format);

    if(QGuiApplication::screens().size() == 2)
    {
        ssvepStimulation->resize(utils::getScreenSize());
        ssvepStimulation->setScreen(QGuiApplication::screens().last());
    }
    else
    {
        ssvepStimulation->resize(QSize(1366, 768)); // temporaty size;
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
