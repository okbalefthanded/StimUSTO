#ifndef ARABICSPELLER_H
#define ARABICSPELLER_H
//
#include "speller.h"

class ArabicSpeller : public Speller
{
    Q_OBJECT


public:
    void startFlashing();

protected slots:
    void pauseFlashing();

};

#endif // ARABICSPELLER_H
