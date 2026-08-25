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


#include <QProtobufSerializer>
#include "incomingmsg.h"

IncomingMsg::IncomingMsg(QObject *parent)
    : QObject{parent}
{}

IncomingMsg::~IncomingMsg()
{
}

void IncomingMsg::Start(QTcpSocket *socket){
    socket_ = socket;
    QObject::connect(socket_, &QAbstractSocket::readyRead, this, &IncomingMsg::ReadyRead);
}

void IncomingMsg::ProcessMsg(const QByteArray &complete_msg) {
    QProtobufSerializer serializer;
    if (!serializer.deserialize(&msg_, complete_msg)) {
        qInfo() << "Failed to parse message:" << serializer.lastErrorString();
        return;
    }
    qInfo() << complete_msg.size() << " bytes read from socket ";
    emit InMsgParsed();
}
void IncomingMsg::ReadyRead() {
    qInfo() << "Ready To Read";
    msgStream_.append(socket_->readAll());

    while (true) {
        if (msgStream_.size() < 4) {
            // Length header hasn't fully arrived yet.
            break;
        }

        const quint32 msg_len =
            (static_cast<quint8>(msgStream_[0]) << 24) |
            (static_cast<quint8>(msgStream_[1]) << 16) |
            (static_cast<quint8>(msgStream_[2]) << 8) |
            (static_cast<quint8>(msgStream_[3]));

        if (msgStream_.size() < static_cast<int>(4 + msg_len)) {
            // Payload hasn't fully arrived yet.
            break;
        }

        const QByteArray complete_msg = msgStream_.mid(4, msg_len);
        msgStream_.remove(0, 4 + msg_len);

        ProcessMsg(complete_msg);
    }
}

nw::remote::Message* IncomingMsg::GetMsg()
{
    return &msg_;
}
