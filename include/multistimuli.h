#ifndef MULTISTIMULI_H
#define MULTISTIMULI_H

#include "speller.h"
#include "erp.h"

class MultiStimuli : public Speller
{

protected slots:
    void startFlashing();
    void pauseFlashing();
};

#endif // MULTISTIMULI_H
