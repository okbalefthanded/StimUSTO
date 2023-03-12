#include <QtMath>
#include "circularlayout.h"

CircularLayout::CircularLayout(QWidget *parent)
{

}


QSize CircularLayout::sizeHint() const
{
    return QSize(1000, 700);
}

void CircularLayout::setGeometry(const QRect &rect)
{
     QLayout::setGeometry(rect);
     const qreal radius = 340;// 346; // 150;//qMin(rect.width(), rect.height()) / 2.0;
     const QPointF center(683, 384);//1366x768 = rect.center();

     for (int i = 0; i < itemList.size(); ++i)
     {
         const qreal angle = i * 2 * M_PI / itemList.size();
         const qreal x = center.x() + radius * qCos(angle);
         const qreal y = center.y() + radius * qSin(angle);
         const QSizeF size = itemList[i]->sizeHint();
         const QRectF itemRect(QPointF(x - size.width() / 2.0, y - size.height() / 2.0), size);
         itemList[i]->setGeometry(itemRect.toRect());
     }
}

void CircularLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}

QLayoutItem *CircularLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem *CircularLayout::takeAt(int index)
{
    if(index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    return nullptr;
}

int CircularLayout::count() const
{
    return itemList.size();
}

CircularLayout::~CircularLayout()
{
    QLayoutItem *item;
    while((item = takeAt(0)))
        delete item;
}

