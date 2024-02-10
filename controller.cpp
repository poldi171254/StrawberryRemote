#include "controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent},
      socket_(new QTcpSocket(this))
{
    QObject::connect(socket_, &QIODevice::readyRead, this,&Controller::Ready);
    QObject::connect(socket_, &QAbstractSocket::errorOccurred, this,&Controller::Error);
}

Controller::~Controller()
{
    statusWindow_->~ConnectionStatus();
}

void Controller::Init(QString ipAddr, qint16 port)
{
    ipAddr_ = ipAddr;
    port_ = port;
    hostAddr_.setAddress(ipAddr_);
    //server_ = new QTcpServer;
    socket_->connectToHost(hostAddr_,port_);
    socket_->waitForConnected();
    statusWindow_ = new ConnectionStatus();
    if(socket_->isOpen()){
        statusWindow_->DisplayText("Connected to Server");
    }
    else {
        statusWindow_->DisplayText("Problem Connecting to Server");
    }

    statusWindow_->show();
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

void Controller::Ready()
{
    qDebug() << "Controller ready";
}

void Controller::Error(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << "Remote Host closed";
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "The host was not found. Please check the host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "The connection was refused by the peer. ";
        break;
    default:
        qDebug() << "The following error occurred: %1." << socket_->errorString();
    }
}
