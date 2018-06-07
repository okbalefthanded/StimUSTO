#include "ovmarkersender.h"
#include "ovtk_stimulations.h"
#include <QMessageBox>
#include <QDataStream>


OVMarkerSender::OVMarkerSender(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this))
{

}

OVMarkerSender::~OVMarkerSender()
{
    if (m_socket->isOpen())
    {
        m_socket->close();
        qDebug() << "Socket Closed";

    }
}

bool OVMarkerSender::Connect(QString t_asAddress, QString t_asTcpTagPort)
{


    qDebug() << "Connection Adress: " << t_asAddress;
    qDebug() << "Connection Port : " << t_asTcpTagPort;

    m_socket->connectToHost(t_asAddress, t_asTcpTagPort.toUShort());

    if(m_socket->waitForConnected())
    {
        qDebug() << "Socket Connection State Connected";
        m_connectedOnce = true;
        return true;
    }

    else
    {
        qDebug() << "Socket Connection State Not Connected";
        m_connectedOnce = false;
        return false;
    }
}

bool OVMarkerSender::sendStimulation(uint64_t t_ovStimulation)

{

    if ( !m_connectedOnce)
    {
        qDebug()<< "Not sending Tag ";
        return false;
    }

    if (!m_socket->isOpen())
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
        streamovs << timeStamp << t_ovStimulation << timeStamp;
        m_socket->write(byteovStimulation);
        m_socket->waitForBytesWritten();
    }

    catch(...)
    {

        qDebug() <<"Send Stimulation, Issue With writting Stimulus";
    }

    return true;

}

bool OVMarkerSender::connectedOnce() const
{
    return m_connectedOnce;
}

void OVMarkerSender::setConnectedOnce(bool t_connectedOnce)
{
    m_connectedOnce = t_connectedOnce;
}
