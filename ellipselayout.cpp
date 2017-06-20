//
#include <QtMath>
#include <QDebug>
//
#include "ellipselayout.h"

EllipseLayout::EllipseLayout(QString position)
{
    orientation = position;

}

EllipseLayout::~EllipseLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

QSize EllipseLayout::sizeHint() const
{
    QSize s(0,0);
    int n = list.count();
    if (n > 0)
        s = QSize(100,100); //start with a nice default size
    int i = 0;
    while (i < n) {
        QLayoutItem *o = list.at(i);
        s = s.expandedTo(o->sizeHint());
        ++i;
    }
    return s + n*QSize(spacing(), spacing());

}

QSize EllipseLayout::minimumSize() const
{
    QSize s(0,0);
    int n = list.count();
    int i = 0;
    while (i < n) {
        QLayoutItem *o = list.at(i);
        s = s.expandedTo(o->minimumSize());
        ++i;
    }
    return s + n*QSize(spacing(), spacing());

}

void EllipseLayout::setGeometry(const QRect &r)
{
    QLayout::setGeometry(r);
    // ellipse related variables
    double start, step;
    int a,b;
    double phi;
    // rect related variables
    int w = r.width() - (list.count() - 1) * spacing();
    int h = r.height() - (list.count() - 1) * spacing();
    int x = w / 2;
    int y = h / 2;
    int spacex = r.x();
    int spacey = r.y();

    if( orientation.compare("upper", Qt::CaseInsensitive) == 0)
    {
        a = 700;
        b = 300;
//                a = 650;
//                b = 275;
        start = qDegreesToRadians(-100.0);
        step =  -(  M_PI / list.size() );
    }
    else
    {
        a = 400;
        b = 150;
        start = qDegreesToRadians(-130.0 / 2);
        step =  (   M_PI / list.size() );
    }

    if (list.size() == 0)
        return;

    int itemIndex = 0;
    while (itemIndex < list.size()) {
        QLayoutItem *o = list.at(itemIndex);
        phi = start + itemIndex * step;
        // ellipse : x = a sin(phi), y = b cos(phi)
        QRect geom(spacex + qRound(a*qSin(phi)) + x, spacey + qRound(b*qCos(phi)), w, h);
        o->setGeometry(geom);
        ++itemIndex;
    }
}

void EllipseLayout::addItem(QLayoutItem *item)
{
    list.append(item);
}

QLayoutItem *EllipseLayout::itemAt(int index) const
{
    return list.value(index);
}

QLayoutItem *EllipseLayout::takeAt(int index)
{
    return index >= 0 && index < list.size() ? list.takeAt(index) : 0;
}

int EllipseLayout::count() const
{
    return list.size();
}
