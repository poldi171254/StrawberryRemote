#include "outgoingmsg.h"

OutgoingMsg::OutgoingMsg(QObject *parent)
    : QObject{parent},
    msg_(new nw::remote::Message)
{
}

OutgoingMsg::~OutgoingMsg()
{
}

bool OutgoingMsg::BeginMsgExchange(QTcpSocket *socket)
{
    socket_ = socket;
    RequestSongInfo();
    return  statusOk_;
}

void OutgoingMsg::RequestSongInfo()
{
    msg_->Clear();
    msg_->set_type(nw::remote::MSG_TYPE_REQUEST_SONG_INFO);
    msg_->mutable_request_song_metadata()->set_send(true);
    Send();
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



