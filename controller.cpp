#include "controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent},
      connection_(new Connection(this))
{
    QObject::connect(connection_, &Connection::ConnectionError, this, &Controller::ConnectionError);

}

Controller::~Controller()
{
    statusWindow_->~ConnectionStatus();
}

void Controller::Init(QString ipAddr, int port)
{
    ipAddr_ = ipAddr;
    port_ = port;
    statusOk_ = connection_->Init(ipAddr_, port_);

    if(statusOk_){
        statusWindow_ = new ConnectionStatus();
        QObject::connect(statusWindow_,&ConnectionStatus::Continue, this, &Controller::Continue);
        QObject::connect(statusWindow_,&ConnectionStatus::Cancel, this, &Controller::Cancel);

        statusWindow_->activateWindow();
        statusWindow_->DisplayText("Connected to Server at " + ipAddr_ + " Port " + QString::number(port_));
        statusWindow_->show();
    }

}


QString Controller::GetIpAddress()
{
    bool found = false;
    QList<QHostAddress> hostList = QNetworkInterface::allAddresses();

    for (const QHostAddress &address : hostList)
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false && !found){
            // NOTE: this code currently only takes the first ip address it finds
            // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // qInfo("Warning: The code only picks the first IPv4 address");
            found = true;
            ipAddr_ = address.toString();
        }
    }
    return ipAddr_;
}

void Controller::Cancel()
{
    qInfo("Controller Cancel ");
    statusWindow_->close();
}

void Controller::Continue()
{
    qInfo("Controller Continue");
    DoWork();
}

void Controller::DoWork()
{
    player_ = new Player;
    player_->activateWindow();
    player_->show();

    connection_->Connect();

}

void Controller::Ready()
{
    qDebug() << "Controller ready";
}

void Controller::ConnectionError()
{
    qInfo("Controller ConnectionError");
    Cancel();
}

