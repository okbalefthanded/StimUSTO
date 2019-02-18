#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H
//
#include "serializer.h"
//
//
class JsonSerializer : public Serializer
{
public:
    JsonSerializer();

    void save(const Serializable& serializable,
              const QString& filepath,
              const QString& rootName) override;
    void load(Serializable& serializable,
              const QString& filepath) override;
}   ;

#endif // JSONSERIALIZER_H
