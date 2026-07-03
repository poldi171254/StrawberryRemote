#include "incomingmsg.h"

IncomingMsg::IncomingMsg(QObject *parent)
    : QObject{parent},
    msg_(new nw::remote::Message)
{}

IncomingMsg::~IncomingMsg()
{
}

void IncomingMsg::Start(QTcpSocket *socket){
    socket_ = socket;
    QObject::connect(socket_, &QAbstractSocket::readyRead, this, &IncomingMsg::ReadyRead);
}


void IncomingMsg::ProcessMsg(const QByteArray &complete_msg) {
    msgString_ = complete_msg.toStdString();
    msg_->ParseFromString(msgString_);
    qInfo() << msgString_.size() << " bytes read from socket ";
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
    return msg_;
}
