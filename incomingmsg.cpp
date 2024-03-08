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

void IncomingMsg::ProcessMsg(){
    msgString_ = msgStream_.toStdString();
    msg_->ParseFromString(msgString_);
    emit InMsgParsed();
}

void IncomingMsg::ReadyRead(){
    qInfo() << "Ready To Read";
    msgStream_ = socket_->readAll();
    int ln = msgStream_.length();
    if (msgStream_.length() > 0) ProcessMsg();
}

nw::remote::Message* IncomingMsg::GetMsg()
{
    return msg_;
}
