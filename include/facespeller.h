#ifndef FACESPELLER_H
#define FACESPELLER_H

#include "speller.h"
#include "erp.h"

class FaceSpeller : public Speller
{
public:
    void startFlashing();
    virtual void initSpeller(ERP *prdg);
private:
    void stimulationFace();
    void stimulationColoredFace();
    void stimulationInvertedFace();
    void stimulationInvertedColoredFace();
};
#endif // FACESPELLER_H
