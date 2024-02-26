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

void Connection::DisplayError()
{

    errorWindow_ = new MessageWindow();
    //QObject::connect(statusWindow_,&ConnectionStatus::Continue, this, &Controller::Continue);
    QObject::connect(errorWindow_,&MessageWindow::Ok,this, &Connection::Close);
    errorWindow_->DisplayText(errorMsg_);
    errorWindow_->activateWindow();
    errorWindow_->show();
}

bool Connection::Init(QString ipAddr, int port)
{
    statusOk_ = false;
    ipAddr_ = ipAddr;
    port_ = port;
    hostAddr_.setAddress(ipAddr_);
    socket_->connectToHost(hostAddr_,port_);
    if (socket_->waitForConnected(1000)){
        qDebug() << "Socket connected";
        statusOk_ = true;
    }

    return statusOk_;
}

bool Connection::Connect()
{
    msgOut_ = new OutgoingMsg;
    msgOut_->Start(socket_);

    qDebug() << "Sending Connect request";
    return statusOk_;
}

void Connection::Close()
{
    errorWindow_->close();
}

void Connection::Error(QAbstractSocket::SocketError socketError)
{
    statusOk_ = false;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << "Remote Host closed";
        errorMsg_ = "Remote Host closed";
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "The host was not found. Please check the host name and port settings.";
        errorMsg_ = "The host was not found. Please check the host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "The connection was refused by the peer. ";
        errorMsg_ = "The connection was refused by the peer. ";
        break;
    default:
        qDebug() << "The following error occurred: %1." << socket_->errorString();
        errorMsg_ = "The following error occurred: %1." + socket_->errorString();
    }
    DisplayError();
}
