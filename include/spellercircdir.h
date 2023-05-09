#ifndef SPELLERCIRCDIR_H
#define SPELLERCIRCDIR_H

#include "speller.h"

class SpellerCircDir : public Speller
{
     Q_OBJECT
public:
    explicit SpellerCircDir(QWidget *parent = nullptr);
    ~SpellerCircDir();

protected:
    void startFlashing();
    void virtual stimulationColoredFace();
    void feedback();

protected:
    void createLayout();
};


#endif // SPELLERCIRCDIR_H
