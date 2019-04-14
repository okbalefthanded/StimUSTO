#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent, QString fileName);
    ~Logger();
    void setShowDateTime(bool value);

private:
    QFile *file;
    bool m_showDate;

signals:

public slots:
    void write(const QString &value);
};



#endif // LOGGER_H
