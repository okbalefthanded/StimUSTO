#ifndef LAYOUT_H
#define LAYOUT_H

#include <QObject>
#include <QColor>
//

class Layout : public QObject
{
    Q_OBJECT

public:
    explicit Layout(QString shape, QColor background,
                    QColor flickerColor, QColor centerColor,
                    float flickerdimension, int vSpace,
                    int hSpace
                   );
    ~Layout();

    QString shape() const;
    void setShape(const QString &newShape);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &newBackgroundColor);

    QColor flickerColor() const;
    void setFlickerColor(const QColor &newFlickerColor);

    QColor centerColor() const;
    void setCenterColor(const QColor &newCenterColor);

    float flickerRadius() const;
    void setFlickerRadius(float newFlickerRadius);

    int verticalSpace() const;
    void setVerticalSpace(int newVerticalSpace);

    int horizontalSpace() const;
    void setHorizontalSpace(int newHorizontalSpace);

protected:
    QString m_shape;
    QColor m_backgroundColor = Qt::black;
    QColor m_flickerColor = Qt::white;
    QColor m_centerColor  = Qt::red;
    float m_flickerDimension = 3.7;
    int m_verticalSpace   = 0;
    int m_horizontalSpace = 0;
};

#endif // LAYOUT_H
