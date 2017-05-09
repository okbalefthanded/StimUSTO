#include <QWindow>
#include "mvepspeller.h"
#include "ui_mvepspeller.h"


mVEPSpeller::mVEPSpeller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mVEPSpeller)
{

    ui->setupUi(this);
    this->show();
    this->windowHandle()->setScreen(qApp->screens().last());
    this->showFullScreen();

    //TODO
    //init Speller

    //Qgraphic scene
    //speller symbols

}

mVEPSpeller::~mVEPSpeller()
{
    delete ui;
}



