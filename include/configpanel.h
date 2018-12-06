#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H
//
#include <QMainWindow>
#include <QUdpSocket>
//
#include "ovmarkersender.h"
#include "speller.h"
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

private slots:
    void on_connectOvAsBtn_clicked();
    void on_initSpeller_clicked();
    void on_startSpeller_clicked();
    void on_pauseSpeller_clicked();
    void on_stopSpeller_clicked();
    void on_quitSpeller_clicked();
    void on_initSSVEP_clicked();
    void on_initHybrid_clicked();


private:
    void initSpeller(Speller *t_sp, int t_spellerType);
    Ui::ConfigPanel *ui;
};

#endif // CONFIGPANEL_H
