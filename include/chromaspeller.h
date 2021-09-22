#ifndef CHROMASPELLER_H
#define CHROMASPELLER_H
//
#include<QMap>
#include "ui_spellerform.h"
//
#include "speller.h"
#include "erp.h"

class ChromaSpeller : public Speller
{
    Q_OBJECT

public:
    explicit ChromaSpeller(QWidget *parent = 0);
    // ~ChromaSpeller();

protected slots:
    void startFlashing();
    // void pauseFlashing();
    void createLayout();

private:
    QMap<int, QString> m_iconMap{  {1, "1_blue.png"},
                                    {2, "2_blue.png"},
                                    {3, "3_blue.png"},
                                    {4, "4_yellow.png"},
                                    {5, "5_yellow.png"},
                                    {6, "6_yellow.png"},
                                    {7, "7_magenta.png"},
                                    {8, "8_magenta.png"},
                                    {9, "9_magenta.png"}
                                   };


};
#endif // CHROMASPELLER_H

