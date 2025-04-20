#ifndef LAYOUT_H
#define LAYOUT_H

#include <QVariantMap>
#include <QObject>
#include <QColor>
//

class Layout : public QObject
{
    Q_OBJECT

public:
    explicit Layout(QString shape, QColor background,
                    QString flickerShape, QColor flickerColor,
                    QColor centerColor, float flickerdimension,
                    int vSpace, int hSpace
                   );
    Layout(QVariantMap settings);
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

    float flickerDimension() const;
    void setFlickerDimension(float newFlickerDimension);



    QString flickerShape() const;
    void setFlickerShape(const QString &newFlickerShape);

protected:
    QString m_shape;
    QColor m_backgroundColor = Qt::black;
    QString m_flickerShape = "circle";
    QColor m_flickerColor = Qt::white;
    QColor m_centerColor  = Qt::red;
    float m_flickerDimension = 3.7;
    int m_verticalSpace   = 0; // in pixels
    int m_horizontalSpace = 0; // in pixels

    void createFlickerDimension(QVariantMap &settings);

};

class Grid: public Layout
{
    Q_OBJECT
public:
    explicit Grid(QString shape, QColor background, QString fShape,
                    QColor flickerColor, QColor centerColor,
                    float flickerdimension, int vSpace,
                    int hSpace, int rows, int cols
                    );
    Grid(QVariantMap settings);
    ~Grid();


public:
    int rows() const;
    void setRows(int newRows);

    int cols() const;
    void setCols(int newCols);

protected:
    int m_rows;
    int m_cols;
};

class Circular : public Layout
{
    Q_OBJECT

public:
    explicit Circular(QString shape, QColor background, QString fShape,
                      QColor flickerColor, QColor centerColor,
                      float flickerdimension, int vSpace,
                      int hSpace, int radius
                      );

    Circular(QVariantMap settings);
    ~Circular();

    float radius() const;
    void setRadius(float newRadius);

protected:
    float m_radius;

};


#endif // LAYOUT_H
