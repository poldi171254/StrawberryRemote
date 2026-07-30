#include "controller.h"
#include "RemoteMessages.qpb.h"


Controller::Controller(QObject *parent)
    : QObject{parent},
    connection_(new Connection(this)),
    msgIn_(new IncomingMsg(this)),
    msgOut_(new OutgoingMsg(this)),
    player_(new Player)
{
    QObject::connect(connection_, &Connection::ConnectionError, this, &Controller::ConnectionError);
    QObject::connect(msgIn_, &IncomingMsg::InMsgParsed, this, &Controller::IncomingMsgReceived);
    QObject::connect(player_, &Player::Playing, this, &Controller::Play);
    QObject::connect(player_, &Player::Paused, this, &Controller::Pause);
    QObject::connect(player_, &Player::PlayPrevisous, this, &Controller::Previous);
    QObject::connect(player_, &Player::PlayNext, this, &Controller::Next);
    QObject::connect(player_, &Player::Finished, this, &Controller::Finish);

    countdown_timer_ = new QTimer(this);
    countdown_timer_->setInterval(1000);
    QObject::connect(countdown_timer_, &QTimer::timeout, this, &Controller::TickRemaining);
}

Controller::~Controller() = default;

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
    if (statusWindow_) {
        statusWindow_->close();
    }
}

void Controller::Continue()
{
    if (statusWindow_) {
        statusWindow_->DisplayText("Connected - waiting for server to accept...");
    }
    MsgHandler();
}

void Controller::MsgHandler()
{
    socket_ = connection_->GetSocket();
    msgIn_->Start(socket_);
    msgOut_->Start(socket_);
    QObject::connect(socket_, &QAbstractSocket::disconnected, this, &Controller::SocketDisconnected);
    msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_CONNECT);
}

// Shows a status/error message in whichever window the user is currently
// looking at: the player window once the handshake has completed, otherwise
// the connection status window.
void Controller::ShowMessage(const QString &text)
{
    if (player_ && player_->isVisible()) {
        player_->SetMessage(text);
        player_->update();
    }
    else if (statusWindow_) {
        statusWindow_->DisplayText(text);
    }
    else {
        qInfo() << text;
    }
}

// Formats remaining_seconds_ as m:ss and pushes it to the player window.
void Controller::UpdateRemainingDisplay()
{
    if (!player_) return;

    if (remaining_seconds_ > 0) {
        player_->SetRemaining(QString("%1:%2")
                                  .arg(remaining_seconds_ / 60)
                                  .arg(remaining_seconds_ % 60, 2, 10, QChar('0')));
    }
    else {
        player_->SetRemaining("0:00");
    }
}

// Called once per second while the track is playing.
void Controller::TickRemaining()
{
    if (remaining_seconds_ > 0) {
        --remaining_seconds_;
    }
    if (remaining_seconds_ <= 0) {
        countdown_timer_->stop();
    }
    UpdateRemainingDisplay();
}

void Controller::IncomingMsgReceived()
{
    nw::remote::Message *msg = msgIn_->GetMsg();

    // The player window is destroyed when the user exits; ignore anything that
    // arrives while we are shutting down.
    if (!player_) return;

    switch (msg->type()) {
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REPLY_SONG_INFO: {
        const nw::remote::ResponseSongMetadata response = msg->responseSongMetadata();
        const nw::remote::SongMetadata song = response.songMetadata();
        player_->SetTitle(song.title());
        player_->SetArtist(song.artist());
        player_->SetAlbum(song.album());
        player_->SetTrack(QString::number(song.track()));
        player_->SetYear(song.stryear());
        player_->SetGenre(song.genre());
        player_->SetPlayCount(QString::number(song.playcount()));
        player_->SetSongLength(song.songlength());

        // Resync the countdown from the server's authoritative position.
        const quint32 length_seconds = response.lengthSeconds();
        const quint32 position_seconds = response.positionSeconds();
        remaining_seconds_ = (length_seconds > position_seconds)
                                 ? static_cast<int>(length_seconds - position_seconds)
                                 : 0;
        UpdateRemainingDisplay();

        switch (response.playerState()) {
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_PLAYING:
            player_->SetMessage("Playing");
            if (remaining_seconds_ > 0) {
                countdown_timer_->start();
            }
            break;
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_PAUSED:
            player_->SetMessage("Paused");
            countdown_timer_->stop();
            break;
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_IDLE:
            player_->SetMessage("Idle");
            countdown_timer_->stop();
            break;
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_ERROR:
            player_->SetMessage("Error");
            countdown_timer_->stop();
            break;
        case nw::remote::PlayerStateGadget::PlayerState::PLAYER_STATUS_EMPTY:
        default:
            player_->SetMessage("No song selected");
            countdown_timer_->stop();
            remaining_seconds_ = 0;
            UpdateRemainingDisplay();
            break;
        }
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_ENGINE_STATE_CHANGE:
        switch (msg->engineStateChange().state()) {
        case nw::remote::EngineStateGadget::EngineState::ENGINE_STATE_PLAYING:
            player_->SetMessage("Playing");
            // The reply to this restarts the countdown with a fresh position.
            msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_SONG_INFO);
            break;
        case nw::remote::EngineStateGadget::EngineState::ENGINE_STATE_PAUSED:
            player_->SetMessage("Paused");
            countdown_timer_->stop();
            break;
        default:
            player_->SetMessage("Stopped");
            countdown_timer_->stop();
            break;
        }
        break;
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_DISCONNECT: {
        countdown_timer_->stop();
        QString reason_text;
        switch (msg->requestDisconnect().reasonDisconnect()) {
        case nw::remote::ReasonDisconnectGadget::ReasonDisconnect::REASON_DISCONNECT_VERSION_MISMATCH:
            reason_text = "Server rejected this client: protocol version too old";
            break;
        case nw::remote::ReasonDisconnectGadget::ReasonDisconnect::REASON_DISCONNECT_UNKNOWN_MSGTYPE:
            reason_text = "Server rejected an unsupported request";
            break;
        case nw::remote::ReasonDisconnectGadget::ReasonDisconnect::REASON_DISCONNECT_SERVER_SHUTDOWN:
            reason_text = "Strawberry has been closed";
            break;
        default:
            reason_text = "Server closed the connection";
            break;
        }
        qInfo() << reason_text;
        expecting_disconnect_ = true;
        ShowMessage(reason_text);
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_RESPONSE_CONNECT:
        if (msg->responseConnect().accepted()) {
            qInfo() << "Handshake accepted, server protocol version" << msg->version();
            if (statusWindow_) {
                statusWindow_->close();
            }
            player_->activateWindow();
            player_->show();
            msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_SONG_INFO);
        }
        else {
            expecting_disconnect_ = true;
            ShowMessage("Server refused the connection");
        }
        break;
    default:
        qInfo("Not sure what the MsgType is ");
        ShowMessage("Unexpected message from server");
        break;
    }

    if (player_) {
        player_->update();
    }
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
    expecting_disconnect_ = true;
    countdown_timer_->stop();
    msgOut_->Send(nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_REQUEST_FINISH);
    if (player_) {
        player_->deleteLater();
        player_ = nullptr;
    }
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

void Controller::SocketDisconnected()
{
    qInfo("Socket disconnected");
    countdown_timer_->stop();
    // If the server already told us why (rejection, shutdown) or we asked to
    // leave, keep that message rather than overwriting it with a generic one.
    if (!expecting_disconnect_) {
        ShowMessage("Connection to server lost");
    }
}