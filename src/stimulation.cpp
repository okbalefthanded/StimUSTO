//
#include<QTimer>
#include "stimulation.h"


Stimulation::Stimulation()
{
    // initTimers();
    // initFeedbackSocket();
    m_state = trial_state::PRE_TRIAL;
}

void Stimulation::initFeedbackSocket()
{
    m_feedbackSocket = new QUdpSocket(this);
    m_feedbackSocket->bind(QHostAddress::LocalHost, m_feedbackPort);
    connect(m_feedbackSocket, SIGNAL(readyRead()), this, SLOT(receiveFeedback()));
}

void Stimulation::receiveFeedback()
{
    QHostAddress sender;
    quint16 senderPort;
    QByteArray *buffer = new QByteArray();

    buffer->resize(m_feedbackSocket->pendingDatagramSize());

    while(m_feedbackSocket->hasPendingDatagrams())
    {
        m_feedbackSocket->readDatagram(buffer->data(), buffer->size(), &sender, &senderPort);
    }


    if (m_flashingSequence->sequence.length() == 1) // Hybrid stimulation mode
    {
        m_sessionFeedback = buffer->data();
    }
    else
    {
        m_sessionFeedback += buffer->data();
    }
    // }


    m_presentFeedback = true;
}

Paradigm *Stimulation::getSettings() const
{
    return m_settings;
}

void Stimulation::setSettings(Paradigm *newSettings)
{
    m_settings = newSettings;
}

Stimulation::~Stimulation(){}
