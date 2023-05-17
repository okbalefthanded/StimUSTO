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
    QList<QPixmap> m_multStimuli = {QPixmap(":/images/bennabi_face_magenta.png"),
                                    QPixmap(":/images/bennabi_face_blue.png"),
                                    QPixmap(":/images/bennabi_face_red.png"),
                                    QPixmap(":/images/bennabi_face_orange.png")};

protected:
    void createLayout();
};


#endif // SPELLERCIRCDIR_H
