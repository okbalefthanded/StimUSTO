#include "session.h"

Session::Session()
{

    this->show();

    this->windowHandle()->setScreen(qApp->screens().last());
    this->showFullScreen();
    this->setStyleSheet("background-color : black");

    create_layout();

    this->stimTimer = new QTimer(this);
    this->isiTimer = new QTimer(this);
    this->preTrialTimer = new QTimer(this);

    this->stimTimer->setTimerType(Qt::PreciseTimer);
    stimTimer->setSingleShot(true);

    isiTimer->setTimerType(Qt::PreciseTimer);
    isiTimer->setSingleShot(true);

    preTrialTimer->setTimerType(Qt::PreciseTimer);
    preTrialTimer->setInterval(1000);
    preTrialTimer->setSingleShot(true);

    connect( stimTimer, SIGNAL(timeout()), this, SLOT(pauseFlashing()) );
    connect( isiTimer, SIGNAL(timeout()), this, SLOT(startFlashing()) );
    connect( preTrialTimer, SIGNAL(timeout()), this, SLOT(startTrial()) );

    feedback_socket = new QUdpSocket(this);
    feedback_socket->bind(QHostAddress::LocalHost, feedback_port);

    state = PRE_TRIAL;
}
