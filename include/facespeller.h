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
    QPixmap *m_stimuli = new QPixmap(":/images/bennabi_face_red_inverted.png");
    QList<QPixmap> m_multStimuli = {QPixmap(":/images/bennabi_face_magenta_inverted.png"),
                                    QPixmap(":/images/bennabi_face_blue_inverted.png"),
                                    QPixmap(":/images/bennabi_face_red_inverted.png")};

};
#endif // FACESPELLER_H
