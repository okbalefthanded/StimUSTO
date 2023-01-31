#ifndef EXTERNCOMM_H
#define EXTERNCOMM_H

#include<QTcpSocket>
#include "utils.h"

class ExternComm : public QObject
{
    Q_OBJECT

public:
    explicit ExternComm(const QString t_address, const quint16 t_port, const quint8 t_commState);
      ~ExternComm();

    const QString &command() const;
    void setCommand(const QString &newCommand);

    quint16 machinePort() const;
    void setMachinePort(quint16 newMachinePort);

    void communicate(const QString t_command);
    void sendToMachine(const QString t_command);
    void recieveFromMachine();

    const QString &machineAddress() const;
    void setMachineAddress(const QString &newMachineAddress);

private:
    // external communication
    QString m_command = "";
    quint16 m_machinePort = 12347;
    QString m_machineAddress = "";
    QTcpSocket *m_machineSocket;
    quint8 m_commState    = external_comm::DISABLED;
    quint8 m_machineState = machine_state::READY;
    bool m_blocking = false;
};



#endif // EXTERNCOMM_H
