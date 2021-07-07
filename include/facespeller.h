#ifndef FACESPELLER_H
#define FACESPELLER_H

#include "speller.h"
#include "erp.h"

class FaceSpeller : public Speller
{
public:
    void startFlashing();
private:
    void stimulationFace();
    void stimulationColoredFace();
    void stimulationInvertedFace();
    void stimulationInvertedColoredFace();
    void stimulationMismatchFace();
};
#endif // FACESPELLER_H
