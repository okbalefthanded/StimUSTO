#ifndef LAYOUTFACTORY_H
#define LAYOUTFACTORY_H

#include <QVariantMap>
//
#include "layout.h"

class LayoutFactory
{

public:

    static Layout *layoutFactory(const QVariantMap settings)
    {
        QString layoutShape = settings.value("layout_shape").toString();

        if (layoutShape.compare("grid", Qt::CaseInsensitive) == 0)
        {
            Grid *grid = new Grid(settings);
            return grid;
        }

        else if(layoutShape.compare("circular", Qt::CaseInsensitive) == 0)
        {
            Circular *circular = new Circular(settings);
            return circular;
        }

        else
        {
            return nullptr;
        }
    }
};


#endif // LAYOUTFACTORY_H
