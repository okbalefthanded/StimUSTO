#ifndef HYBRIDGRIDSTIMULATION_H
#define HYBRIDGRIDSTIMULATION_H

#include <QWidget>

namespace Ui {
class HybridGridStimulation;
}

class HybridGridStimulation : public QWidget
{
    Q_OBJECT

public:
    explicit HybridGridStimulation(QWidget *parent = 0);
    ~HybridGridStimulation();

private:
    Ui::HybridGridStimulation *ui;
};

#endif // HYBRIDGRIDSTIMULATION_H
