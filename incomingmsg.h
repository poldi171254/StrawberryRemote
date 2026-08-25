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

#ifndef INCOMINGMSG_H
#define INCOMINGMSG_H

#include <QObject>

#include <google/protobuf/message.h>

#include "RemoteMessages.qpb.h"
#include "qtcpsocket.h"

class IncomingMsg : public QObject {
    Q_OBJECT
public:
    explicit IncomingMsg(QObject *parent = nullptr);
    ~IncomingMsg();
    void Start(QTcpSocket*);
    void ProcessMsg(const QByteArray &complete_msg);
    nw::remote::Message* GetMsg();

private slots:
    void ReadyRead();

signals:
    void InMsgParsed();

private:
    nw::remote::Message msg_;
    QTcpSocket *socket_;
    long bytesIn_;
    QByteArray msgStream_;
    std::string msgString_;
    qint32 msgType_;
};

#endif // INCOMINGMSG_H
