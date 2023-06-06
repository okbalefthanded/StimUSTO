#ifndef CIRCULARLAYOUT_H
#define CIRCULARLAYOUT_H

#include <QLayout>

class CircularLayout : public QLayout
{
    Q_OBJECT

public:
    explicit CircularLayout(QWidget *parent = 0);
    ~CircularLayout();

    QSize sizeHint() const override;
    void setGeometry(const QRect &rect) override;

    void addItem(QLayoutItem *item) override;
    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;
    int count() const override;

private:
    QPointF getPoint(int index, QPointF center, qreal radius);
    QList<QLayoutItem *> itemList;

};

#endif // CIRCULARLAYOUT_H
