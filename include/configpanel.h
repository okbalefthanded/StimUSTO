#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H
//
#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
//
#include "ovmarkersender.h"
#include "speller.h"
#include "paradigm.h"
#include "ssvepgl.h"
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
//
//    int stimulation_duration;
//    int isi;
//    int nr_sequence;
//    int nr_trials;
//    int spelling_mode;
//    QString desired_phrase;

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
    void connectSpeller(Speller *t_sp, QTimer *timer);
    void connectSSVEP(SsvepGL *ssvep, QTimer *timer);
    Ui::ConfigPanel *ui;
    QString configFile;
    bool noGui;
};

#endif // CONFIGPANEL_H
