#include <QHostAddress>
#include "externComm.h"

ExternComm::ExternComm(const QString t_address, const quint16 t_port, const quint8 t_commState)
{
    setMachinePort(t_port);
    setMachineAddress(t_address);
    m_commState = t_commState;
    m_machineSocket = new QTcpSocket();

    if (m_commState == external_comm::ENABLED)
    {
        m_machineSocket->connectToHost(QHostAddress(t_address), t_port);

        if(m_machineSocket->waitForConnected())
        {
            qDebug() << "Robot Connection : State Connected";
        }
        else
        {
            qDebug() << "Robot Connection : State Not Connected";
        }
    }

}

void ExternComm::communicate(const QString t_command)
{
    qDebug()<<"Extern com communicate";
    sendToMachine(t_command);
    if(m_blocking == true)
    {
      recieveFromMachine();
    }

}

void ExternComm::sendToMachine(const QString t_command)
{
    setCommand(t_command);

    if(m_commState == external_comm::ENABLED)
    {
        qDebug() << "Sending Feedback to Machine";
        if (!m_machineSocket->isOpen())
        {
            qDebug()<< "Not sending Feedback to Robot : Cannot send feedback socket is not open";
        }

        try
        {
            std::string str = m_command.toStdString();
            const char* p = str.c_str();
            QByteArray byteovStimulation;
            QDataStream streamovs(&byteovStimulation, QIODevice::WriteOnly);
            streamovs.setByteOrder(QDataStream::LittleEndian);
            streamovs.writeRawData(p, m_command.length());
            m_machineSocket->write(byteovStimulation);
            m_machineSocket->waitForBytesWritten();
            qDebug()<< "Feedback sent to Machine";
        }
        catch(...)
        {
            qDebug() <<"Excpetion while Sending Feedback to Machine, Issue With writting Data";
        }

    }
}

void ExternComm::recieveFromMachine()
{
    if (m_commState == external_comm::ENABLED)
    {
        qDebug() << "Recieve State from Robot";
        m_machineSocket->waitForReadyRead();

        QByteArray machineState = m_machineSocket->readAll();

        m_machineState = machineState.toUInt();
        qDebug()<< "Machine State recieved " << m_machineState;

    }
}

const QString &ExternComm::command() const
{
    return m_command;
}

void ExternComm::setCommand(const QString &newCommand)
{
    m_command = newCommand;
}

quint16 ExternComm::machinePort() const
{
    return m_machinePort;
}

void ExternComm::setMachinePort(quint16 newMachinePort)
{
    m_machinePort = newMachinePort;
}


const QString &ExternComm::machineAddress() const
{
    return m_machineAddress;
}

void ExternComm::setMachineAddress(const QString &newMachineAddress)
{
    m_machineAddress = newMachineAddress;
}

ExternComm::~ExternComm()
{

}
