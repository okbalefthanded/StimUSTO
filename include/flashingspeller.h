#ifndef FLASHINGSPELLER_H
#define FLASHINGSPELLER_H

#include "speller.h"
#include "erp.h"
class FlashingSpeller : public Speller
{
    void startFlashing();
public:
    virtual void initSpeller(ERP prdg);
};


#endif // FLASHINGSPELLER_H
