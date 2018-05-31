#ifndef SPELLER_H
#define SPELLER_H

#include <QWidget>

namespace Ui {
class Speller;
}

class Speller : public QWidget
{
    Q_OBJECT

public:
    explicit Speller(QWidget *parent = 0);
    ~Speller();

private:
    Ui::Speller *ui;
};

#endif // SPELLER_H
