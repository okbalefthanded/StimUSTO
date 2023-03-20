#ifndef SPELLERCIRCULAR_H
#define SPELLERCIRCULAR_H

#include "spellersmall.h"

class SpellerCircular : public SpellerSmall
{
     Q_OBJECT
public:
    explicit SpellerCircular(QWidget *parent = nullptr);
    ~SpellerCircular();

protected:
    void startFlashing();
    void virtual stimulationColoredFace();

protected:
    void createLayout();
};


#endif // SPELLERCIRCULAR_H
