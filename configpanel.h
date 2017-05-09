#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

#include <QMainWindow>
#include "mvepspeller.h"
#include "ovmarkersender.h"

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
    bool onStart;
    bool onPause;
    bool onStop;
    bool onQuit;
//
//    int stimulation_duration;
//    int isi;
//    int nr_sequence;
//    int nr_trials;
//    int spelling_mode;
//    QString desired_phrase;

    OVMarkerSender *cTest;



private slots:
    void on_connectOvAsBtn_clicked();
    void on_initSpeller_clicked();
    void on_startSpeller_clicked();
    void on_pauseSpeller_clicked();
    void on_stopSpeller_clicked();
    void on_quitSpeller_clicked();

private:
    Ui::ConfigPanel *ui;


//    mVEPSpeller *speller;

};

#endif // CONFIGPANEL_H
