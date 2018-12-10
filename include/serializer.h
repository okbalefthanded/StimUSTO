#ifndef SERIALIZER_H
#define SERIALIZER_H
//
#include <QString>
//
#include "serializable.h"
//

class Serializer
{
public:
    virtual ~Serializer() {}

    virtual void save (const Serializable& serializable,
                       const QString& filepath,
                       const QString& rootName ="config") = 0;
    virtual void load(Serializable& Serializable,
                      const QString& filepath) = 0;

};

#endif // SERIALIZER_H
