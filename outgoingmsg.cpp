#include "outgoingmsg.h"

OutgoingMsg::OutgoingMsg(QObject *parent)
    : QObject{parent},
    msg_(new nw::remote::Message)
{
}

OutgoingMsg::~OutgoingMsg()
{
}

void OutgoingMsg::Start(QTcpSocket *socket)
{
    socket_ = socket;
}


void OutgoingMsg::RequestSongInfo()
{
    msg_->Clear();
    msg_->set_type(nw::remote::MSG_TYPE_REQUEST_SONG_INFO);
    msg_->mutable_request_song_metadata()->set_send(true);

}

void OutgoingMsg::RequestPlay()
{
    msg_->Clear();
    msg_->set_type(nw::remote::MSG_TYPE_REQUEST_PLAY);
    msg_->mutable_request_play()->set_play(true);

}

void OutgoingMsg::RequestPause()
{
    msg_->Clear();
    msg_->set_type(nw::remote::MSG_TYPE_REQUEST_PAUSE);
    msg_->mutable_request_pause()->set_pause(true);

}

void OutgoingMsg::RequestPrevious()
{
    msg_->Clear();
    msg_->set_type(nw::remote::MSG_TYPE_REQUEST_PREVIOUS);
    msg_->mutable_request_previous_track()->set_previous(true);

}

void OutgoingMsg::RequestNext()
{
    msg_->Clear();
    msg_->set_type(nw::remote::MSG_TYPE_REQUEST_NEXT);
    msg_->mutable_request_next_track()->set_next(true);

}

void OutgoingMsg::RequestFinish()
{
    msg_->Clear();
    msg_->set_type(nw::remote::MSG_TYPE_REQUEST_FINISH);
    msg_->mutable_request_stop()->set_stop(true);

}


void OutgoingMsg::Send(int msgType)
{
    msg_->Clear();

    switch (msgType) {
        case nw::remote::MSG_TYPE_REQUEST_SONG_INFO:
            msg_->set_type(nw::remote::MSG_TYPE_REQUEST_SONG_INFO);
            msg_->mutable_request_song_metadata()->set_send(true);
            break;
        case nw::remote::MSG_TYPE_REQUEST_PLAY:
            msg_->set_type(nw::remote::MSG_TYPE_REQUEST_PLAY);
            msg_->mutable_request_play()->set_play(true);
            break;
        case nw::remote::MSG_TYPE_REQUEST_NEXT:
            msg_->set_type(nw::remote::MSG_TYPE_REQUEST_NEXT);
            msg_->mutable_request_next_track()->set_next(true);
            break;
        case nw::remote::MSG_TYPE_REQUEST_PREVIOUS:
            msg_->set_type(nw::remote::MSG_TYPE_REQUEST_PREVIOUS);
            msg_->mutable_request_previous_track()->set_previous(true);
            break;
        case nw::remote::MSG_TYPE_REQUEST_PAUSE:
            msg_->set_type(nw::remote::MSG_TYPE_REQUEST_PAUSE);
            msg_->mutable_request_pause()->set_pause(true);
            break;
        case nw::remote::MSG_TYPE_REQUEST_FINISH:
            msg_->set_type(nw::remote::MSG_TYPE_REQUEST_FINISH);
            msg_->mutable_request_stop()->set_stop(true);
            break;
        default:
            qInfo() << "Unknown Message type " << msgType;
            break;
    }
    std::string  msgOut_;
/*
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
*/
    msg_->SerializeToString(&msgOut_);

    QByteArray payload = QByteArray::fromStdString(msgOut_);
    const quint32 msg_len = static_cast<quint32>(payload.size());

    QByteArray framed_data;
    framed_data.reserve(4 + payload.size());
    framed_data.append(static_cast<char>((msg_len >> 24) & 0xFF));
    framed_data.append(static_cast<char>((msg_len >> 16) & 0xFF));
    framed_data.append(static_cast<char>((msg_len >> 8) & 0xFF));
    framed_data.append(static_cast<char>(msg_len & 0xFF));
    framed_data.append(payload);

    bytesOut_ = framed_data.size();

    if (socket_->isWritable()) {
        socket_->write(framed_data);
        qInfo() << bytesOut_ << " bytes written to socket " << socket_->socketDescriptor();
        statusOk_ = true;
        msg_->Clear();
    }
    else {
        statusOk_ = false;
    }
}



