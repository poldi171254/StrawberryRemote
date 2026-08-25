/*
 * Strawberry Music Player Client
 * Copyright 2026, Leopold List <leo@zudiewiener.com>
 *
 * The client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 */


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
    msgWindow_ = new MessageWindow();
    QObject::connect(msgWindow_,&MessageWindow::Ok,this, &Connection::Close);
    msgWindow_->DisplayText(errorMsg_);
    msgWindow_->activateWindow();
    msgWindow_->show();
}

QTcpSocket *Connection::GetSocket()
{
    return socket_;
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

void Connection::Close()
{
    msgWindow_->close();
}

void Connection::Error(QAbstractSocket::SocketError socketError)
{
    statusOk_ = false;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        errorMsg_ = "Remote Host closed";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMsg_ = "The host was not found. Please check the host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorMsg_ = "The connection was refused by the peer. ";
        break;
    default:
        errorMsg_ = "The following error occurred: %1." + socket_->errorString();
    }
    DisplayError();
}
