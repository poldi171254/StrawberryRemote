#include <QProtobufSerializer>
#include "outgoingmsg.h"

OutgoingMsg::OutgoingMsg(QObject *parent)
    : QObject{parent},
    socket_(nullptr),
    bytesOut_(0)
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
    Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_SONG_INFO);
}

void OutgoingMsg::RequestPlay()
{
    Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PLAY);
}

void OutgoingMsg::RequestPause()
{
    Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PAUSE);
}

void OutgoingMsg::RequestPrevious()
{
    Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PREVIOUS);
}

void OutgoingMsg::RequestNext()
{
    Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_NEXT);
}

void OutgoingMsg::RequestFinish()
{
    Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_FINISH);
}


void OutgoingMsg::Send(nw::remote::MsgTypeGadget::MsgType msg_type)
{
    msg_ = nw::remote::Message();
    msg_.setType(msg_type);

    switch (msg_type) {
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_SONG_INFO: {
        nw::remote::RequestSongMetadata request;
        request.setSend(true);
        msg_.setRequestSongMetadata(request);
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PLAY: {
        nw::remote::RequestPlay request;
        request.setPlay(true);
        msg_.setRequestPlay(request);
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_NEXT: {
        nw::remote::RequestNextTrack request;
        request.setNext(true);
        msg_.setRequestNextTrack(request);
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PREVIOUS: {
        nw::remote::RequestPreviousTrack request;
        request.setPrevious(true);
        msg_.setRequestPreviousTrack(request);
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PAUSE: {
        nw::remote::RequestPause request;
        request.setPause(true);
        msg_.setRequestPause(request);
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_FINISH: {
        nw::remote::RequestStop request;
        request.setStop(true);
        msg_.setRequestStop(request);
        break;
    }
    default:
        qInfo() << "Unknown Message type";
        return;
    }

    QProtobufSerializer serializer;
    QByteArray payload = serializer.serialize(&msg_);

    if (serializer.lastError() != QAbstractProtobufSerializer::Error::None) {
        qInfo() << "Failed to serialize message:" << serializer.lastErrorString();
        statusOk_ = false;
        return;
    }

    const quint32 msg_len = static_cast<quint32>(payload.size());

    QByteArray framed_data;
    framed_data.reserve(4 + payload.size());
    framed_data.append(static_cast<char>((msg_len >> 24) & 0xFF));
    framed_data.append(static_cast<char>((msg_len >> 16) & 0xFF));
    framed_data.append(static_cast<char>((msg_len >> 8) & 0xFF));
    framed_data.append(static_cast<char>(msg_len & 0xFF));
    framed_data.append(payload);

    bytesOut_ = framed_data.size();

    if (socket_ && socket_->isWritable()) {
        socket_->write(framed_data);
        qInfo() << bytesOut_ << " bytes written to socket " << socket_->socketDescriptor();
        statusOk_ = true;
    }
    else {
        statusOk_ = false;
    }
}