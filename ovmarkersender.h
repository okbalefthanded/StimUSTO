#ifndef OVMARKERSENDER_H
#define OVMARKERSENDER_H

#include <QTcpSocket>
#include <QDebug>
#include <QObject>

class OVMarkerSender : public QObject
{
    Q_OBJECT

public:

    explicit OVMarkerSender(QObject *parent = 0);
    ~OVMarkerSender();
    bool Connect(QString asAddress, QString asTcpTagPort);


signals:

public slots:
    bool sendStimulation(uint64_t ovStimulation);


private :
    QTcpSocket *socket;

protected :
    bool connectedOnce;

};


#endif // OVMARKERSENDER_H
