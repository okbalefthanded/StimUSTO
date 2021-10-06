#ifndef AUDITORYSPELLER_H
#define AUDITORYSPELLER_H

#include<QMap>
#include<QSound>
#include <QSoundEffect>
#include "ui_spellerform.h"
//
#include "speller.h"
#include "erp.h"

class AuditorySpeller : public Speller
{
    Q_OBJECT

public:
    explicit AuditorySpeller(QWidget *parent = 0);
    // ~ChromaSpeller();

protected slots:
    void startFlashing();
    void pauseFlashing();
    void postTrial();
    void createLayout();
    void highlightTarget();
    void refreshTarget();
    void feedback();

private:
    void initSounds();
    // QList<QSound *> *m_soundStim;
     QList<QSoundEffect *> *m_soundStim;
};

#endif // AUDITORYSPELLER_H
