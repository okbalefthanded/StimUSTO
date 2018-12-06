#ifndef FACESPELLER_H
#define FACESPELLER_H

#include "speller.h"

class FaceSpeller : public Speller
{
    void startFlashing();

private:
    void stimulationFace();
    void stimulationColoredFace();
    void stimulationInvertedFace();
    void stimulationInvertedColoredFace();
};
#endif // FACESPELLER_H
