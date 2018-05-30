#include "motionitem.h"
#include <QDebug>

const QString DEFAULT_TEXT = "#";
const QColor DEFAULT_TEXT_COLOR = Qt::black;
const quint8 DEFAULT_TEXT_SIZE = 30;
const QColor DEFAULT_ROW_COLOR = Qt::blue;
const quint16 DEFAULT_SIZE_WIDTH = 120;
const quint16 DEFAULT_SIZE_HEIGHT = 150;

MotionItem::MotionItem()
{
    text = DEFAULT_TEXT;
    textColor = DEFAULT_TEXT_COLOR;
    rowColor = DEFAULT_ROW_COLOR;
}

MotionItem::MotionItem(QString t)
{
    text = t;
    textColor = DEFAULT_TEXT_COLOR;
    rowColor = DEFAULT_ROW_COLOR;
}

QRectF MotionItem::boundingRect() const
{
    return QRectF(0, 0, DEFAULT_SIZE_WIDTH, DEFAULT_SIZE_HEIGHT);
}

void MotionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(textColor);
    QFont font("Times", DEFAULT_TEXT_SIZE);
    QRectF rec = QRectF(0,0,DEFAULT_SIZE_WIDTH, DEFAULT_SIZE_HEIGHT / 2);
    painter->setPen(pen);
    painter->setFont(font);
    painter->drawText(DEFAULT_SIZE_WIDTH / 2, 0, text);
    painter->drawRect(rec);

//    QRectF mid_rect = QRectF(DEFAULT_SIZE_WIDTH / 2, 0, 10, DEFAULT_SIZE_HEIGHT / 2);
//    painter->drawRect(mid_rect);
}
