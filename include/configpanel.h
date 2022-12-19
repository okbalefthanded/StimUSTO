#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H
//
#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <serializable.h>
#include <QOpenGLWindow>
//
#include "ovmarkersender.h"
#include "speller.h"
#include "ssvepgl.h"
#include "paradigm.h"
#include "erp.h"
#include "ssvep.h"
#include "ssvepcircle.h"
#include "phonekeypad.h"
#include "ssvepdirection.h"
#include "ssvepstimulation.h"
//
namespace Ui {
class ConfigPanel;
}

class ConfigPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConfigPanel(QWidget *parent = 0);
    ~ConfigPanel();
//
    bool m_onStart;
    bool m_onPause;
    bool m_onStop;
    bool m_onQuit;

    OVMarkerSender *m_markerSender;
    QUdpSocket *m_startSocket;

    QString getConfigFile() const;
    void setConfigFile(const QString &value);

    bool getNoGui() const;
    void setNoGui(bool value);

private slots:
    void startExperiment();
    void on_connectOvAsBtn_clicked();
    void on_initSpeller_clicked();
    void on_startSpeller_clicked();
    void on_pauseSpeller_clicked();
    void on_stopSpeller_clicked();
    void on_quitSpeller_clicked();
    void on_initSSVEP_clicked();
    void on_initHybrid_clicked();


private:
    void initParadigmJSon(Paradigm *prdg);
    ERP *initParadigmERPGui();
    SSVEP *initParadigmSSVEPGui();
    Speller *createSpeller(int t_spellerType);
    // SsvepGL *createSSVEP(SSVEP *t_ssvep, int t_port);
    // SsvepCircle *createSSVEP(SSVEP *t_ssvep, int t_port);
    // SsvepDirection *createSSVEP(SSVEP *t_ssvep, int t_port);
    // PhoneKeypad *createSSVEP(SSVEP *t_ssvep, int t_port);
    SSVEPstimulation *createSSVEP(SSVEP *t_ssvep, int t_port);
    void connectStimulation(QObject *t_obj);
    void connectParadigm(QObject *pr, QTimer *timer);
    Ui::ConfigPanel *ui;
    QString configFile;
    bool noGui;
};

#endif // CONFIGPANEL_H
