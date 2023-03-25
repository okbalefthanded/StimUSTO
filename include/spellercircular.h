#ifndef SPELLERCIRCULAR_H
#define SPELLERCIRCULAR_H

#include "spellersmall.h"

class SpellerCircular : public SpellerSmall
{
     Q_OBJECT
public:
    explicit SpellerCircular(QWidget *parent = nullptr);
    ~SpellerCircular();
    void showFeedback(QString command, bool correct);

protected:
    void startFlashing();
    void pauseFlashing();
    void highlightTarget();
    void refreshTarget();
    void feedback();
    void postTrial();
    void virtual stimulationColoredFace();
    int getnElements();

protected:
    void createLayout();
};


#endif // SPELLERCIRCULAR_H
