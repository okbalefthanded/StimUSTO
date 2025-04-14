#ifndef NEWCONFIGPANEL_H
#define NEWCONFIGPANEL_H

#include <QMainWindow>
#include <QHostAddress>
//
#include "ovmarkersender.h"
#include "paradigm.h"
#include "utils.h"
//
namespace Ui {
class NewConfigPanel;
}

class NewConfigPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewConfigPanel(QWidget *parent = 0);
    ~NewConfigPanel();

    QString getConfigFilePath() const;
    void setConfigFilePath(const QString &newConfigFilePath);

    OVMarkerSender *m_markerSender;

private slots:
    void on_settingLevel_currentIndexChanged(int index);
    void on_loadConfigBtn_clicked();
    void on_saveConfigBtn_clicked();

    void on_connectOvAsBtn_clicked();

private:
    void setDefaults();
    void serializeParadigmJSON(Paradigm *settings);
    QVariantMap settingsMap();
    void connectMarkerSender(QString t_address="127.0.0.1", QString t_port=config::ovTCPTagPort);
    void initConfigTable(QVariantMap settingsMap);
    QVariantMap configTableToMap();
    Ui::NewConfigPanel *ui;
    QString configFilePath;

};

#endif // NEWCONFIGPANEL_H
