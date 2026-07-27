#include "controller.h"
#include "RemoteMessages.qpb.h"


Controller::Controller(QObject *parent)
    : QObject{parent},
    connection_(new Connection(this)),
    msgIn_(new IncomingMsg),
    msgOut_(new OutgoingMsg),
    player_(new Player)
{
    QObject::connect(connection_, &Connection::ConnectionError, this, &Controller::ConnectionError);
    QObject::connect(msgIn_, &IncomingMsg::InMsgParsed, this, &Controller::IncomingMsgReceived);
    QObject::connect(player_, &Player::Playing, this, &Controller::Play);
    QObject::connect(player_, &Player::Paused, this, &Controller::Pause);
    QObject::connect(player_, &Player::PlayPrevisous, this, &Controller::Previous);
    QObject::connect(player_, &Player::PlayNext, this, &Controller::Next);
    QObject::connect(player_, &Player::Finished, this, &Controller::Finish);

}

Controller::~Controller()
{
    statusWindow_->~ConnectionStatus();
}

void Controller::Init(QString ipAddr, int port)
{
    ipAddr_ = ipAddr;
    port_ = port;
    statusOk_ = connection_->Init(ipAddr_, port_);

    if(statusOk_){
        statusWindow_ = new ConnectionStatus();
        QObject::connect(statusWindow_,&ConnectionStatus::Continue, this, &Controller::Continue);
        QObject::connect(statusWindow_,&ConnectionStatus::Cancel, this, &Controller::Cancel);

        statusWindow_->activateWindow();
        statusWindow_->DisplayText("Connected to Server at " + ipAddr_ + " Port " + QString::number(port_));
        statusWindow_->show();
    }
}


QString Controller::GetIpAddress()
{
    bool found = false;
    QList<QHostAddress> hostList = QNetworkInterface::allAddresses();

    for (const QHostAddress &address : hostList)
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false && !found){
            // NOTE: this code currently only takes the first ip address it finds
            // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // qInfo("Warning: The code only picks the first IPv4 address");
            found = true;
            ipAddr_ = address.toString();
        }
    }
    return ipAddr_;
}

void Controller::Cancel()
{
    statusWindow_->close();
}

void Controller::Continue()
{
    statusWindow_->close();
    MsgHandler();
}

void Controller::MsgHandler()
{
    player_->activateWindow();
    player_->show();

    socket_ = connection_->GetSocket();
    msgIn_->Start(socket_);
    msgOut_->Start(socket_);
    msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_SONG_INFO);
}

void Controller::IncomingMsgReceived()
{
    nw::remote::Message *msg = msgIn_->GetMsg();

    switch (msg->type()) {
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REPLY_SONG_INFO: {
        const nw::remote::SongMetadata song = msg->responseSongMetadata().songMetadata();
        player_->SetTitle(song.title());
        player_->SetArtist(song.artist());
        player_->SetAlbum(song.album());
        player_->SetTrack(QString::number(song.track()));
        player_->SetYear(song.stryear());
        player_->SetGenre(song.genre());
        player_->SetPlayCount(QString::number(song.playcount()));
        player_->SetSongLength(song.songlength());
        switch (msg->responseSongMetadata().playerState()) {
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_PLAYING:
            player_->SetMessage("Playing");
            break;
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_PAUSED:
            player_->SetMessage("Paused");
            break;
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_IDLE:
            player_->SetMessage("Idle");
            break;
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_EMPTY:
            player_->SetMessage("Empty");
            break;
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_ERROR:
            player_->SetMessage("Error");
            break;
        default:
            player_->SetMessage("Unknown");
            break;
        }
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_ENGINE_STATE_CHANGE:
        switch (msg->engineStateChange().state()) {
        case nw::remote::EngineStateGadget::EngineState::ENGINE_STATE_PLAYING:
            player_->SetMessage("Playing");
            msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_SONG_INFO);
            break;
        case nw::remote::EngineStateGadget::EngineState::ENGINE_STATE_PAUSED:
            player_->SetMessage("Paused");
            break;
        default:
            player_->SetMessage("Stopped");
            break;
        }
        break;
    default:
        qInfo("Not sure what the MsgType is ");
        player_->SetMessage("Not sure that the current song is, please press the Play Button");
        break;
    }
    player_->update();
    player_->show();
}

void Controller::Play()
{
    msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PLAY);

}

void Controller::Pause()
{
    msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PAUSE);

}

void Controller::Next()
{
    msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_NEXT);
}

void Controller::Previous()
{
    msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_PREVIOUS);
}


void Controller::Finish()
{
    msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_FINISH);
    delete player_;
}

void Controller::Ready()
{
    qDebug() << "Controller ready";
}

void Controller::ConnectionError()
{
    qInfo("Controller ConnectionError");
    Cancel();
}