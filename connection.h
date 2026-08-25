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


#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "messagewindow.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);
    ~Connection();
    void DisplayError();
    QTcpSocket *GetSocket();


public slots:
    bool Init(QString, int);
    void Close();


private slots:
    void Error(QAbstractSocket::SocketError);

signals:
    void ConnectionError();

private:
    QString ipAddr_;
    int port_;
    QTcpSocket *socket_;
    QHostAddress hostAddr_;
    MessageWindow *msgWindow_;
    QString errorMsg_;
    bool statusOk_;

};

#endif // CONNECTION_H
