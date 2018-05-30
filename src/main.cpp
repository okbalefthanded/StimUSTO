#include "configpanel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConfigPanel w;
    w.show();

    return a.exec();
}
