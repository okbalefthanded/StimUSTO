#include "ovmarkersender.h"
#include "ovtk_stimulations.h"
#include <QMessageBox>
#include <QDataStream>


OVMarkerSender::OVMarkerSender(QObject *parent)
    : QObject(parent),socket(new QTcpSocket(this))
{

}

OVMarkerSender::~OVMarkerSender()
{
    if (socket->isOpen())
    {
        socket->close();
        qDebug() << "Socket Closed";

    }

}

bool OVMarkerSender::Connect(QString asAddress, QString asTcpTagPort)
{

    qDebug() << "Connection Adress: " << asAddress;
    qDebug() << "Connection Port : " << asTcpTagPort.toInt();
    socket->connectToHost(asAddress,asTcpTagPort.toInt());

    if(socket->waitForConnected())
    {
        qDebug() << "Socket Connection State Connected";
        OVMarkerSender::connectedOnce = true;
        return true;
    }

    else
    {
        qDebug() << "Socket Connection State Not Connected";
        OVMarkerSender::connectedOnce = false;
        return false;
    }
}

bool OVMarkerSender::sendStimulation(uint64_t ovStimulation)

{

    if ( !connectedOnce)
    {
        qDebug()<< "Not sending Tag ";
        return false;
    }

    if (!socket->isOpen())
    {

        qDebug()<< "Not sending Tag : Send Stimulation Cannot send stimulation socket is not open";
        return false;
    }

    uint64_t timeStamp = 0;
    try

    {
        QByteArray byteovStimulation;
        QDataStream streamovs(&byteovStimulation, QIODevice::WriteOnly);
        streamovs.setByteOrder(QDataStream::LittleEndian);
        streamovs << timeStamp << ovStimulation << timeStamp;
        OVMarkerSender::socket->write(byteovStimulation);
        OVMarkerSender::socket->waitForBytesWritten();
    }

    catch(...)
    {

        qDebug() <<"Send Stimulation, Issue With writting Stimulus";
    }

    return true;

}
