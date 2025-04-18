#ifndef SPELLERFACTORY_H
#define SPELLERFACTORY_H

#include "speller.h"
#include "flashingspeller.h"

class SpellerFactory {

public:
    static Speller *createSpeller(const QString spellerType)
    {
        if (spellerType == "")
        {
            FlashingSpeller *flashSpeller = new FlashingSpeller();
            return flashSpeller;
        }
    }
};

#endif // SPELLERFACTORY_H
