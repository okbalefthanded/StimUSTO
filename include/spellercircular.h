#ifndef SPELLERCIRCULAR_H
#define SPELLERCIRCULAR_H

#include "spellersmall.h"

class SpellerCircular : public SpellerSmall
{
     Q_OBJECT
public:
    explicit SpellerCircular(QWidget *parent = 0);
    ~SpellerCircular();
protected slots:
    void startFlashing();
protected:
    void createLayout();
};


#endif // SPELLERCIRCULAR_H
