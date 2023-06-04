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
#include "hybriderp.h"
#include "hybridssvep.h"
#include "utils.h"
#include "paradigm.h"
//
#include "flashingspeller.h"
#include "facespeller.h"
#include "arabicspeller.h"
#include "spellersmall.h"
#include "spellercircular.h"
#include "spellercircdir.h"
#include "multistimuli.h"
#include "chromaspeller.h"
#include "auditoryspeller.h"
#include "erp.h"
#include "ssvep.h"
#include "hybrid.h"
#include "doubleerp.h"
#include "doublessvep.h"
#include "ssvepcircle.h"
#include "phonekeypad.h"
#include "ssvepdirection.h"
#include "ssvepstimulation.h"
#include "ssvepdircircle.h"
#include "jsonserializer.h"
#include "ovtk_stimulations.h"
//
ConfigPanel::ConfigPanel(QWidget *parent) : QMainWindow(parent), ui(new Ui::ConfigPanel)
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

    else if(paradigmType == paradigm_type::DOUBLE_ERP)
    {
        initDoubleERP();
    }
    else if(paradigmType == paradigm_type::DOUBLE_SSVEP)
    {
        initDoubleSSVEP();
    }
}

// init speller
/**
 * @brief ConfigPanel::on_initSpeller_clicked
 */
void ConfigPanel::on_initSpeller_clicked()
{
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

        Speller *speller = createSpeller(spellerType, 12345);
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
        return;
    }

    else
    {
        //
        QTimer *launchTimer = new QTimer();
        launchTimer->setInterval(10000);
        launchTimer->setSingleShot(true);

        // SsvepGL *ssvepStimulation = createSSVEP(ssvepParadigm, 12345);
        // SsvepCircle *ssvepStimulation = createSSVEP(ssvepParadigm, 12345);
        // SsvepDirection *ssvepStimulation = createSSVEP(ssvepParadigm, 12345);
        SSVEPstimulation *ssvepStimulation = createSSVEP(ssvepParadigm, 12345);
        // PhoneKeypad *ssvepStimulation = createSSVEP(ssvepParadigm, 12345);
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

        Speller *speller = createSpeller(hybridParadigm->m_ERPparadigm->stimulationType(), 12345);
        speller->setERP(hybridParadigm->m_ERPparadigm);
        speller->setPresentFeedback(false);
        // SsvepGL *ssvepStimulation = createSSVEP(hybridParadigm->m_SSVEPparadigm, 12346);
        // SsvepGL *ssvepStimulation = new SsvepGL(hybridParadigm->m_SSVEPparadigm, 12346);
        // SsvepCircle *ssvepStimulation = createSSVEP(hybridParadigm->m_SSVEPparadigm, 12346);
        // SsvepDirection *ssvepStimulation = createSSVEP(hybridParadigm->m_SSVEPparadigm, 12346);
        SSVEPstimulation *ssvepStimulation = createSSVEP(hybridParadigm->m_SSVEPparadigm, 12346);
        ssvepStimulation->setPresentFeedback(false);

        HybridStimulation *hybrid = new HybridStimulation(hybridParadigm, speller, ssvepStimulation);
        connectParadigm(hybrid, launchTimer);
        connect(hybrid, SIGNAL(experimentEnd()), this, SLOT(on_quitSpeller_clicked()));
    }
}

void ConfigPanel::initDoubleERP()
{
    DoubleERP *hybridParadigm = new DoubleERP();

    if(noGui)
    {
        initParadigmJSon(hybridParadigm);
    }
    else
    {
        ERP *FirstParadigm  = initParadigmERPGui();
        ERP *SecondParadigm = initParadigmERPGui();
        *hybridParadigm = DoubleERP(FirstParadigm, SecondParadigm);
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

        Speller *speller = createSpeller(hybridParadigm->m_1stParadigm->stimulationType(), 12345);
        speller->setERP(hybridParadigm->m_1stParadigm);
        speller->setPresentFeedback(false);

        Speller *speller2 = createSpeller(hybridParadigm->m_2ndParadigm->stimulationType(), 12346);
        speller2->setERP(hybridParadigm->m_2ndParadigm);
        speller2->setPresentFeedback(false);

        HybridERP *hybrid = new HybridERP(hybridParadigm, speller, speller2);
        connectParadigm(hybrid, launchTimer);
        connect(hybrid, SIGNAL(experimentEnd()), this, SLOT(on_quitSpeller_clicked()));
    }
}

void ConfigPanel::initDoubleSSVEP()
{
    DoubleSSVEP *hybridParadigm = new DoubleSSVEP();

    if(noGui)
    {
        initParadigmJSon(hybridParadigm);
    }
    else
    {
        SSVEP *FirstParadigm  = initParadigmSSVEPGui();
        SSVEP *SecondParadigm = initParadigmSSVEPGui();
        *hybridParadigm = DoubleSSVEP(FirstParadigm, SecondParadigm);
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

        SSVEPstimulation *ssvepSpeller1 = createSSVEP(hybridParadigm->m_1stParadigm, 12345);
        ssvepSpeller1->setPresentFeedback(false);

        SSVEPstimulation *ssvepSpeller2 = createSSVEP(hybridParadigm->m_2ndParadigm, 12346);
        ssvepSpeller2->setPresentFeedback(false);

        HybridSSVEP *hybrid = new HybridSSVEP(hybridParadigm, ssvepSpeller1, ssvepSpeller2);
        connectParadigm(hybrid, launchTimer);
        connect(hybrid, SIGNAL(experimentEnd()), this, SLOT(on_quitSpeller_clicked()));
    }
}

ERP *ConfigPanel::initParadigmERPGui()
{
    int spellerType = ui->spellerType->currentIndex();
    ERP *erpParadigm = new ERP(ui->spellingModeChoices->currentIndex(),
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

Speller *ConfigPanel::createSpeller(int t_spellerType, quint16 t_port)
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
    case speller_type::MISMATCH:
    {
        FaceSpeller *faceSpeller = new FaceSpeller();
        connectStimulation(faceSpeller);
        return faceSpeller;
    }

    case speller_type::CHROMA:
    {
        ChromaSpeller *chromaSpeller = new ChromaSpeller();
        connectStimulation(chromaSpeller);
        return chromaSpeller;
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

    case speller_type::SMALL:
    case speller_type::SMALL_FLASH:
    case speller_type::SMALL_FACE:
    case speller_type::SMALL_IFACE:
    case speller_type::SMALL_ICFACE:
    {
        SpellerSmall *smallSpeller = new SpellerSmall(nullptr, t_port);
        connectStimulation(smallSpeller);
        return smallSpeller;
    }
    case speller_type::SMALL_CIRCLE:
    {
        SpellerCircular *spellerCircle = new SpellerCircular(nullptr, t_port);
        connectStimulation(spellerCircle);
        return spellerCircle;
    }

    case speller_type::CIRC_DIR:
    {
        SpellerCircDir *spellerCircleDir = new SpellerCircDir();
        connectStimulation(spellerCircleDir);
        return spellerCircleDir;
    }
        /*
    case speller_type::AUDITORY:
    {
        AuditorySpeller *auditorySpeller = new AuditorySpeller();
        connectStimulation(auditorySpeller);
        return auditorySpeller;
    }
    */
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
                              speller_type::SSVEP_CIRCLE,
                              "",
                              "127.0.0.1",
                              SSVEPNrElements,
                              ui->Frequencies->text(),
                              ui->SSVEP_Stimulation->currentIndex());
    return ssvepParadigm;
}

// SsvepGL *ConfigPanel::createSSVEP(SSVEP *t_ssvep, int t_port)
// PhoneKeypad *ConfigPanel::createSSVEP(SSVEP *t_ssvep, int t_port)
// SsvepCircle *ConfigPanel::createSSVEP(SSVEP *t_ssvep, int t_port)
// SsvepDirection *ConfigPanel::createSSVEP(SSVEP *t_ssvep, int t_port)
SSVEPstimulation *ConfigPanel::createSSVEP(SSVEP *t_ssvep, int t_port)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapInterval(1); // vsync on
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer); //
    format.setVersion(3, 0); // ANGLE supports ES 3.0, higher versions raise exceptions

    // format.setVersion(4, 5); // HP ProBook
    // format.setVersion(4, 6); // Intel UHD 620
    // PhoneKeypad *ssvepStimulation = new PhoneKeypad(t_ssvep, t_port);
    // SsvepGL *ssvepStimulation = new SsvepGL(t_ssvep, t_port);
    // SsvepCircle *ssvepStimulation = new SsvepCircle(t_ssvep, t_port);
    // SsvepDirection *ssvepStimulation = new SsvepDirection(t_ssvep, t_port);
    SSVEPstimulation *ssvepStimulation;

    switch(t_ssvep->stimulationType())
    {
    case speller_type::SSVEP_DIRECTIONS:
    {
        ssvepStimulation = new SsvepDirection(t_ssvep, t_port);
        break;
    }
    case speller_type::SSVEP_CIRCLE:
    case speller_type::SSVEP_GRID:
    {
        ssvepStimulation = new SsvepCircle(t_ssvep, t_port);
        break;
    }

    case speller_type::SSVEP_DIRCIRCLE:
    {
        ssvepStimulation = new SsvepDirectionCircle(t_ssvep, t_port);
        break;
    }
    }

    ssvepStimulation->setFormat(format);

    if(QGuiApplication::screens().size() == 2)
    {
        ssvepStimulation->resize(utils::getScreenSize());
        ssvepStimulation->setScreen(QGuiApplication::screens().last());
    }
    else
    {
     //   ssvepStimulation->resize(QSize(1366, 768)); // temporaty size;
         ssvepStimulation->resize(QSize(1920, 1080)); // temporaty size;
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
