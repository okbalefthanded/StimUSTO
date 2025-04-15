//
#include "layout.h"
//

Layout::Layout(QString shape, QColor background,
               QColor flickerColor, QColor centerColor,
               float flickerdimension, int vSpace,
               int hSpace) : QObject(),
    m_shape(shape), m_backgroundColor(background),
    m_flickerColor(flickerColor), m_centerColor(centerColor),
    m_flickerDimension(flickerdimension), m_verticalSpace(vSpace),
    m_horizontalSpace(hSpace)
{}

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


// Destructor
Layout::~Layout(){}


