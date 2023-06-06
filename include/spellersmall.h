#ifndef SPELLERSMALL_H
#define SPELLERSMALL_H

#include "ui_spellerform.h"
#include "speller.h"
#include "erp.h"

class SpellerSmall : public Speller
{

    Q_OBJECT

public:
    explicit SpellerSmall(QWidget *parent = nullptr, quint16 t_port=12345);
    SpellerSmall(quint16 t_port);
    ~SpellerSmall();
    virtual void showFeedback(QString command, bool correct);

protected slots:
    void feedback();
    void postTrial();
    void startFlashing();
    void pauseFlashing();
    void createLayout();

protected:
    void virtual flashing();
    void virtual stimulationFace();
    void virtual stimulationColoredFace();
    void virtual stimulationInvertedFace();
    void virtual stimulationInvertedColoredFace();
    bool isTarget();
    void highlightTarget();
    void refreshTarget();
    QString getElemColor(int t_index);

};

#endif // SPELLERSMALL_H
