#ifndef ELLIPSELAYOUT_H
#define ELLIPSELAYOUT_H
//
#include <QLayout>
#include <QList>
#include <QtWidgets>
//
class EllipseLayout : public QLayout
{
public:
    EllipseLayout(QString position);
    ~EllipseLayout();

    // QLayoutItem interface
public:
    QSize sizeHint() const;
    QSize minimumSize() const;
    void setGeometry(const QRect &r);

    // QLayout interface
public:
    void addItem(QLayoutItem *item);
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    int count() const;

private:
    QList<QLayoutItem*> list;
    QString orientation;
};
#endif // ELLIPSELAYOUT_H
