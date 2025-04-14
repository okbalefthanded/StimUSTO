#include <QJsonDocument>
#include <QFile>
#include <QDebug>
//
#include "jsonserializer.h"
//
JsonSerializer::JsonSerializer(){}

void JsonSerializer::save(const Serializable& serializable, const QString &filepath)
{
    QJsonDocument doc = QJsonDocument::fromVariant(serializable.toVariantSave());
    QFile file(filepath);

    if(file.open(QFile::WriteOnly))
    {
        qDebug() << Q_FUNC_INFO << doc.toJson();
        file.write(doc.toJson());
    }
    else
    {
        qDebug() << "Error saving file:" << file.errorString();
    }
    file.close();
}

void JsonSerializer::load(Serializable& serializable, const QString &filepath)
{
    QFile file(filepath);
    if (file.open(QFile::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        serializable.fromVariant(doc.toVariant());
    }
    else
    {
        qDebug() << "Error opening file:" << file.errorString();
    }
    file.close();
}

