#include "hybridgridstimulation.h"
#include "ui_hybridgridstimulation.h"

HybridGridStimulation::HybridGridStimulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HybridGridStimulation)
{
    ui->setupUi(this);
}

HybridGridStimulation::~HybridGridStimulation()
{
    delete ui;
}
