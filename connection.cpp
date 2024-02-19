#include "connection.h"

Connection::Connection(QObject *parent)
    : QObject{parent},
      socket_(new QTcpSocket(this))
{
    QObject::connect(socket_, &QAbstractSocket::errorOccurred, this,&Connection::Error);
}


Connection::~Connection()
{
}

void Connection::Init(QString ipAddr, int port)
{
    ipAddr_ = ipAddr;
    port_ = port;
    hostAddr_.setAddress(ipAddr_);
    socket_->connectToHost(hostAddr_,port_);
    if (socket_->waitForConnected(1000)){
        qDebug() << "Socket connected";
    }
    else {
        qDebug() << "Socket not connected";
    }

}

void Connection::Connect()
{
    msg_ = new (spb::remote::Message);
    msg_->setType(spb::remote::MsgTypeGadget::CONNECT);
    spb::remote::RequestConnect* connect_ = new spb::remote::RequestConnect;
    connect_->setSendCurrentSong(true);
    msg_->setRequestConnect(*connect_);
    qDebug() << "Sending Connect request";
}

void Connection::Error(QAbstractSocket::SocketError socketError)
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
    emit ConnectionError();
}
