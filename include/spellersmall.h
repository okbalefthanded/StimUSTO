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
    ~SpellerSmall();

protected slots:
    void preTrial();
    void feedback();
    void postTrial();
    void startFlashing();
    void pauseFlashing();
    void createLayout();

protected:

    bool isTarget();
    void highlightTarget();
    void refreshTarget();

};

#endif // SPELLERSMALL_H
