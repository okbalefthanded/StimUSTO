#include <QGuiApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
//
#include "ui_newconfigpanel.h"
#include "newconfigpanel.h"
#include "jsonserializer.h"
#include "paradigm.h"

NewConfigPanel::NewConfigPanel(QWidget *parent) : QMainWindow(parent), ui(new Ui::NewConfigPanel)
{
    ui->setupUi(this);
    setDefaults();
    connectMarkerSender("127.0.0.1", "15631");
    Paradigm defaultSettings;
    serializeParadigmJSON(&defaultSettings);
    initConfigTable(defaultSettings.map());
}


//
void NewConfigPanel::serializeParadigmJSON(Paradigm *settings)
{
    // connectMarkerSender("127.0.0.1", "15631");
    JsonSerializer jSerializer;
    jSerializer.load(*settings, configFilePath);

    // qDebug()<< Q_FUNC_INFO << settings->map()->keys();
    // qDebug()<< Q_FUNC_INFO << settings->map()->values();
}


void NewConfigPanel::connectMarkerSender(QString t_address, QString t_port)
{
    qDebug()<< Q_FUNC_INFO << t_address << t_port;

    m_markerSender = new OVMarkerSender(this);
    if(!m_markerSender->Connect(t_address, t_port))
    // if(!m_markerSender->Connect("127.0.0.1", "15631"))
    {
        qDebug()<< "Connection to OpenVibe acquisition server failed";
    }
}


// inits

void NewConfigPanel::initConfigTable(QVariantMap settingsMap)
{
    QString settingsLevel = ui->settingLevel->currentText();

    /*
    QVector<QString> basicSettings = {"language", "experimentMode",
                                      "paradigmType", "desiredPhrase",
                                      "nrSequences", "stimulationDuration",
                                      "breakDuration"};
    */
    int rows = 0;

    ui->ConfigFile->setRowCount(settingsMap.keys().length());
    ui->ConfigFile->verticalHeader()->setVisible(false);

    foreach (const QString &key, settingsMap.keys())
    {
        ui->ConfigFile->setItem(rows, 0, new QTableWidgetItem(key));
        ui->ConfigFile->setItem(rows, 1, new QTableWidgetItem(settingsMap.value(key).toString()));
        QTableWidgetItem *item = ui->ConfigFile->item(rows, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        if(settingsLevel.compare("basic", Qt::CaseInsensitive) == 0)
        {
            if(!Paradigm::basicSettings.contains(key))
            {
                ui->ConfigFile->setRowHidden(rows, true);
            }
        }

        ++rows;
    }

    ui->ConfigFile->horizontalHeader()->setDefaultSectionSize(500);
}

QVariantMap NewConfigPanel::configTableToMap()
{
    QVariantMap variablesMap;
    QVariant value;
    QString key = "";
    QTableWidgetItem* keyItem   = nullptr;
    QTableWidgetItem* valueItem = nullptr;
    int rows = ui->ConfigFile->rowCount();

    for(int i=0; i<rows; ++i)
    {
        keyItem   = ui->ConfigFile->item(i, 0);
        valueItem = ui->ConfigFile->item(i, 1);

        key = keyItem->data(Qt::DisplayRole).toString();

        if (valueItem)
        {
            value = valueItem->data(Qt::DisplayRole);
        }
        else
        {
            value = QVariant(); // Store as null variant if no item in the second column
        }

        variablesMap[key] = value;
    }

    return variablesMap;
}

// UI elements manipulations
void NewConfigPanel::on_connectOvAsBtn_clicked()
{
    QString ovAsAddress  = ui->addressOvAs->text();
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


void NewConfigPanel::on_settingLevel_currentIndexChanged(int index)
{
    int n_settings = ui->ConfigFile->rowCount();


    if(ui->settingLevel->currentText().compare("advanced", Qt::CaseInsensitive) == 0)
    {
        for(int i=0; i< n_settings  ;++i)
        {
            ui->ConfigFile->setRowHidden(i, false);
        }
    }

    else
    {
        for(int i=0; i< n_settings  ;++i)
        {
            QTableWidgetItem *item = ui->ConfigFile->item(i, 0);
            if(!Paradigm::basicSettings.contains(item->text()))
            {
                ui->ConfigFile->setRowHidden(i, true);
            }
        }
    }
}


void NewConfigPanel::on_loadConfigBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Config File"),
                                                    utils::configFolderPath(),
                                                    tr("Config Files (*.json)"));
    setConfigFilePath(fileName);
    initConfigTable(settingsMap());
}


void NewConfigPanel::on_saveConfigBtn_clicked()
{
    //
    JsonSerializer jSerializer;
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    "Save Config File",
                                                    utils::configFolderPath(),
                                                    tr("Config Files (*.json)"));

    Paradigm *settings = new Paradigm();
    QVariantMap variablesMap = configTableToMap();

    settings->setMap(variablesMap);
    jSerializer.save(*settings, fileName);
}


// getters and setters
QString NewConfigPanel::getConfigFilePath() const
{
    return configFilePath;
}

void NewConfigPanel::setConfigFilePath(const QString &newConfigFilePath)
{
    configFilePath = newConfigFilePath;
}

void NewConfigPanel::setDefaults()
{
    QString desiredPath = utils::configFolderPath();
    desiredPath += "/default.json";
    setConfigFilePath(desiredPath);
}


QVariantMap NewConfigPanel::settingsMap()
{
    Paradigm *settings = new Paradigm();
    JsonSerializer jSerializer;
    jSerializer.load(*settings, configFilePath);

    return settings->map();
}


// destructor
NewConfigPanel::~NewConfigPanel()
{
    delete ui;
}





