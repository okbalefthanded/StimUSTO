#include <QtMath>
#include "circularlayout.h"
#include "utils.h"
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

     const QSize screenSize = utils::getScreenSize();
     const qreal radius = screenSize.width() / 4.5; // 5 //4;
     const QPointF center(screenSize.width() / 2.0, screenSize.height() / 2.0); //1366x768 screensize
     QSize size;
     QRectF itemRect;

     for (int i = 0; i < itemList.size(); ++i)
     {
         size = itemList[i]->sizeHint();
         itemRect = QRectF(getPoint(i, center, radius), size);
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

QPointF CircularLayout::getPoint(int index, QPointF center, qreal radius)
{
    qreal angle, x, y = 0;
    QSize size;
    angle = (index * 2 * M_PI) / itemList.size();
    x = center.x() + radius * qCos(angle);
    y = center.y() - radius * qSin(angle); // the screen plan is different than the OpenGL (0,0) at center
    size = itemList[index]->sizeHint();
    return QPointF(x - size.width() / 2.0, y - size.height() / 2.0);
}

CircularLayout::~CircularLayout()
{
    QLayoutItem *item;
    while((item = takeAt(0)))
        delete item;
}

