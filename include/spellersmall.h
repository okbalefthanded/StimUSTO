#ifndef SPELLERSMALL_H
#define SPELLERSMALL_H

#include "ui_spellerform.h"
#include "speller.h"
#include "erp.h"

class SpellerSmall : public Speller
{
 Q_OBJECT
public:
    explicit SpellerSmall(QWidget *parent = 0);
    SpellerSmall(int i);
    ~SpellerSmall();
    virtual void showFeedback(QString command, bool correct);

protected slots:
    void preTrial();
    void feedback();
    void postTrial();
    void startFlashing();
    void pauseFlashing();
    void createLayout();
    //
    void flashing();
    void stimulationFace();
    void stimulationColoredFace();
    void stimulationInvertedFace();
    void stimulationInvertedColoredFace();


protected:
    bool isTarget();
    void highlightTarget();
    void refreshTarget();

private:
    QString getElemColor(int t_index);
};

#endif // SPELLERSMALL_H
