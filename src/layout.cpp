//
#include "layout.h"
//
// Layout Super Class
Layout::Layout(QString shape, QColor background, QString flickerShape,
               QColor flickerColor, QColor centerColor,
               float flickerdimension, int vSpace,
               int hSpace) : QObject(),
    m_shape(shape), m_backgroundColor(background),
    m_flickerShape(flickerShape), m_flickerColor(flickerColor),
    m_centerColor(centerColor), m_flickerDimension(flickerdimension),
    m_verticalSpace(vSpace), m_horizontalSpace(hSpace)
{}

Layout::Layout(QVariantMap settings)
{
    m_shape = settings.value("layout_shape").toString();
    m_backgroundColor = settings.value("background_color").toString();
    m_flickerShape    = settings.value("flicker_shape").toString();
    createFlickerDimension(settings);
    m_flickerColor    = settings.value("flicker_color").toString();
    m_centerColor     = settings.value("center_color").toString();
    m_verticalSpace   = settings.value("vertical_space").toInt();
    m_horizontalSpace = settings.value("horizontal_space").toInt();
}


// Getters and Setters
QString Layout::shape() const
{
    return m_shape;
}

void Layout::setShape(const QString &newShape)
{
    m_shape = newShape;
}

QColor Layout::backgroundColor() const
{
    return m_backgroundColor;
}

void Layout::setBackgroundColor(const QColor &newBackgroundColor)
{
    m_backgroundColor = newBackgroundColor;
}

QColor Layout::flickerColor() const
{
    return m_flickerColor;
}

void Layout::setFlickerColor(const QColor &newFlickerColor)
{
    m_flickerColor = newFlickerColor;
}

QColor Layout::centerColor() const
{
    return m_centerColor;
}

void Layout::setCenterColor(const QColor &newCenterColor)
{
    m_centerColor = newCenterColor;
}

int Layout::verticalSpace() const
{
    return m_verticalSpace;
}

void Layout::setVerticalSpace(int newVerticalSpace)
{
    m_verticalSpace = newVerticalSpace;
}

int Layout::horizontalSpace() const
{
    return m_horizontalSpace;
}

void Layout::setHorizontalSpace(int newHorizontalSpace)
{
    m_horizontalSpace = newHorizontalSpace;
}

float Layout::flickerDimension() const
{
    return m_flickerDimension;
}

void Layout::setFlickerDimension(float newFlickerDimension)
{
    m_flickerDimension = newFlickerDimension;
}

void Layout::createFlickerDimension(QVariantMap &settings)
{
    if(m_flickerShape.compare("square", Qt::CaseInsensitive) == 0)
    {
        m_flickerDimension = settings.value("flicker_length").toFloat();
    }

    else if(m_flickerShape.compare("circle", Qt::CaseInsensitive) == 0)
    {
        m_flickerDimension = settings.value("flicker_radius").toFloat();
    }
}

QString Layout::flickerShape() const
{
    return m_flickerShape;
}

void Layout::setFlickerShape(const QString &newFlickerShape)
{
    m_flickerShape = newFlickerShape;
}

// Destructor
Layout::~Layout(){}


// Sub Classes : grid, circular

// Grid
Grid::Grid(QString shape, QColor background,
           QString fShape, QColor flickerColor,
           QColor centerColor, float flickerdimension,
           int vSpace, int hSpace, int rows, int cols):
    Layout(shape, background, fShape, flickerColor, centerColor,
             flickerdimension, vSpace, hSpace),
    m_rows(rows), m_cols(cols)
{}

Grid::Grid(QVariantMap settings) : Layout(settings)
{
    m_rows = settings.value("rows").toInt();
    m_cols = settings.value("columns").toInt();
}

int Grid::rows() const
{
    return m_rows;
}

void Grid::setRows(int newRows)
{
    m_rows = newRows;
}

int Grid::cols() const
{
    return m_cols;
}

void Grid::setCols(int newCols)
{
    m_cols = newCols;
}

Grid::~Grid(){}
// Circular

Circular::Circular(QString shape, QColor background, QString fShape,
                   QColor flickerColor, QColor centerColor,
                   float flickerdimension, int vSpace,
                   int hSpace, int radius):
    Layout(shape, background, fShape, flickerColor, centerColor,
             flickerdimension, vSpace, hSpace), m_radius(radius)
{}

Circular::Circular(QVariantMap settings): Layout(settings)
{
    m_radius = settings.value("layout_radius").toFloat();
}

float Circular::radius() const
{
    return m_radius;
}

void Circular::setRadius(float newRadius)
{
    m_radius = newRadius;
}

Circular::~Circular(){}
