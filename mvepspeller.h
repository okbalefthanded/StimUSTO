#ifndef MVEPSPELLER_H
#define MVEPSPELLER_H

#include <QWidget>

namespace Ui {
class mVEPSpeller;
}

class mVEPSpeller : public QWidget
{
    Q_OBJECT

public:
    explicit mVEPSpeller(QWidget *parent = 0);
    ~mVEPSpeller();

private slots:


private:
    Ui::mVEPSpeller *ui;



};

#endif // MVEPSPELLER_H
