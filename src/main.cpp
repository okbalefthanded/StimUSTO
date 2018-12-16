//
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <iostream>
#include <QTimer>
#include <QObject>
//
#include "configpanel.h"
#include "jsonserializer.h"
//
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Laresi BCI");
    QApplication::setApplicationVersion("0.1.0-beta");

    QCommandLineParser *optionParser = new QCommandLineParser();

    optionParser->setApplicationDescription("Stimulation platform for OpenVibe");
    optionParser->addHelpOption();
    optionParser->addVersionOption();

    optionParser->addOptions({
                               {"nogui", "start without showing config panel", "False"},
                               {{"f","file"}, "", "configFile in JSON format", "configfile.json"},
                             });
    optionParser->process(app);

    ConfigPanel w;

    if(optionParser->value("nogui")=="True")
    {
        qDebug()<< "nogui" << optionParser->value("nogui");
        qDebug()<< "file" << optionParser->value("file");
        w.setConfigFile(optionParser->value("file"));
        w.setNoGui(true);
        w.hide();
        QTimer *launchTimer = new QTimer();
        launchTimer->setInterval(1000);
        launchTimer->setSingleShot(true);

        // QObject::connect(launchTimer, SIGNAL(timeout()), &w, SLOT(on_initSpeller_clicked()));
        QObject::connect(launchTimer, SIGNAL(timeout()), &w, SLOT(startExperiment()));
        launchTimer->start();
    }
    else
    {
        w.setConfigFile("");
        w.setNoGui(false);
        w.show();
    };

    return app.exec();
}
