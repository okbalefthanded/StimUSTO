#ifndef MOTIONITEM_H
#define MOTIONITEM_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPainter>

class MotionItem : public QGraphicsItem
{
public:
    MotionItem();
    MotionItem(QString t);
    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
                QWidget *widget);
private:
    QString text;
    QColor textColor;
    QColor rowColor;
};

#endif // MOTIONITEM_H
