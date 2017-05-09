#ifndef OVRECEIVER_H
#define OVRECEIVER_H

#include <QObject>
#include <QUdpSocket>

class ovreceiver : public QObject
{
    Q_OBJECT
public:
    explicit ovreceiver(QObject *parent = 0);

private:
    QUdpSocket *feedbackSocket;

signals:

public slots:
};

#endif // OVRECEIVER_H
