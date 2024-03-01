#include "outgoingmsg.h"

OutgoingMsg::OutgoingMsg(QObject *parent)
    : QObject{parent}
{
}

OutgoingMsg::~OutgoingMsg()
{
}

bool OutgoingMsg::Start(QTcpSocket *socket)
{
    socket_ = socket;
    msg_ = new nw::remote::Message;
    msg_->set_type(nw::remote::MSG_TYPE_CONNECT);
    msg_->set_version(1);
    msg_->mutable_request_connect()->set_send_current_song(true);
    Send();
    return  statusOk_;
}


void OutgoingMsg::Send()
{

    std::string  msgOut_;

    msg_->SerializeToString(&msgOut_);


    bytesOut_ = msg_->ByteSizeLong();

    if(socket_->isWritable()){

        socket_->write(QByteArray::fromStdString(msgOut_));
        qInfo() << socket_->bytesToWrite() << " bytes written to socket " << socket_->socketDescriptor();
        statusOk_ = true;
        msg_->Clear();
    }
    else {
        statusOk_ = false;
    }
}



