#ifndef ARABICSPELLER_H
#define ARABICSPELLER_H
//
#include "ui_spellerform.h"
//
#include "speller.h"
//
class ArabicSpeller : public Speller
{
    Q_OBJECT

public:
   explicit ArabicSpeller(QWidget *parent = 0);
    ~ArabicSpeller();

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

#endif // ARABICSPELLER_H
