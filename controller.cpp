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
    QObject::connect(player_, &Player::PlaylistTabSelected, this, &Controller::PlaylistTabSelected);
    QObject::connect(player_, &Player::SongDoubleClicked, this, &Controller::SongDoubleClicked);
    QObject::connect(player_, &Player::AddCurrentSongToPlaylist, this, &Controller::AddCurrentSongToPlaylist);
    QObject::connect(player_, &Player::RemoveSongFromPlaylist, this, &Controller::RemoveSongFromPlaylist);
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

// Pushes the current column headers / previous / current / upcoming rows to the player window.
void Controller::UpdateQueueDisplay()
{
    if (player_) {
        player_->SetQueue(current_column_headers_, previous_rows_, current_row_, has_current_row_, upcoming_rows_);
    }
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
            // The current/upcoming rows may have shifted; refresh the queue too.
            if (has_viewed_playlist_&& has_active_playlist_ && viewed_playlist_id_ == active_playlist_id_) {
                msgOut_->RequestPlaylistSongs(active_playlist_id_, 25);
            }
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
            msgOut_->RequestInitialInfo();
        }
        else {
            expecting_disconnect_ = true;
            ShowMessage("Server refused the connection");
        }
        break;
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_RESPONSE_INITIAL_INFO: {
        const nw::remote::ResponseSongMetadata response = msg->responseInitialInfo().songInfo();

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

        const nw::remote::ResponsePlaylists playlists_response = msg->responseInitialInfo().playlists();
        playlist_names_.clear();
        playlist_ids_.clear();
        active_playlist_tab_index_ = -1;
        has_active_playlist_ = false;
        int idx = 0;
        for (const nw::remote::PlaylistInfo &pl : playlists_response.playlists()) {
            playlist_names_.append(pl.name());
            playlist_ids_.append(pl.id_proto());
            if (pl.isPlaying()) {
                active_playlist_id_ = pl.id_proto();
                active_playlist_tab_index_ = idx;
                has_active_playlist_ = true;
            }
            ++idx;
        }
        player_->SetPlaylists(playlist_names_, playlist_ids_, active_playlist_tab_index_);
        if (has_active_playlist_) {
            viewed_playlist_id_ = active_playlist_id_;
            has_viewed_playlist_ = true;
            msgOut_->RequestPlaylistSongs(active_playlist_id_, 25);
        }
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_RESPONSE_PLAYLIST_SONGS: {
        const nw::remote::ResponsePlaylistSongs response = msg->responsePlaylistSongs();
        if (has_viewed_playlist_ && response.playlistId() != viewed_playlist_id_) {
            break;  // stale response for a playlist we've since navigated away from
        }

        QList<ColumnInfo> new_headers;
        for (const nw::remote::ColumnInfo &col : response.columns()) {
            ColumnInfo info;
            info.name = col.name();
            info.is_numeric = col.isNumeric();
            new_headers.append(info);
        }
        if (new_headers != current_column_headers_) {
            // Visible columns changed on the desktop mid-session: old cached
            // rows would no longer line up against the new headers.
            previous_rows_.clear();
            current_column_headers_ = new_headers;
        }

        const QList<nw::remote::PlaylistSongRow> rows = response.rows();
        QueueRowData new_current;
        bool new_has_current = false;
        QList<QueueRowData> new_upcoming;

        if (!rows.isEmpty()) {
            new_current.values = rows.first().values();
            new_current.row_index = rows.first().rowIndex();
            new_has_current = true;
            for (int i = 1; i < rows.size(); ++i) {
                QueueRowData r;
                r.values = rows.at(i).values();
                r.row_index = rows.at(i).rowIndex();
                new_upcoming.append(r);
            }
        }

        if (has_current_row_ && (!new_has_current || current_row_.values != new_current.values)) {
            previous_rows_.append(current_row_);
            while (previous_rows_.size() > kMaxPreviousRows) {
                previous_rows_.removeFirst();
            }
        }

        current_row_ = new_current;
        has_current_row_ = new_has_current;
        upcoming_rows_ = new_upcoming;
        UpdateQueueDisplay();
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_RESPONSE_PLAY_SONG: {
        const nw::remote::ResponsePlaySong response = msg->responsePlaySong();
        if (response.accepted() && has_viewed_playlist_) {
            // Refreshes the queue view for whatever playlist we just played
            // from. Note: this does not yet update playlist_ids_/active state
            // if playing a previously-inactive playlist just made it active -
            // that needs PLAYLIST_ACTIVATED handling, not yet wired up.
            msgOut_->RequestPlaylistSongs(viewed_playlist_id_, 25);
        }
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_PLAYLIST_ACTIVATED: {
        const nw::remote::PlaylistActivated activated = msg->playlistActivated();
        active_playlist_id_ = activated.playlistId();
        has_active_playlist_ = true;

        const int idx = static_cast<int>(playlist_ids_.indexOf(active_playlist_id_));
        active_playlist_tab_index_ = idx;
        player_->SetPlaylists(playlist_names_, playlist_ids_, active_playlist_tab_index_);

        // SetPlaylists() blocks signals while moving the tab selection, so
        // PlaylistTabSelected() won't fire on its own here - follow the
        // newly-active playlist explicitly.
        viewed_playlist_id_ = active_playlist_id_;
        has_viewed_playlist_ = true;
        previous_rows_.clear();
        current_row_ = QueueRowData();
        has_current_row_ = false;
        upcoming_rows_.clear();
        current_column_headers_.clear();
        UpdateQueueDisplay();
        msgOut_->RequestPlaylistSongs(viewed_playlist_id_, 25);
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_PLAYLIST_CHANGED: {
        const nw::remote::PlaylistChanged changed = msg->playlistChanged();
        if (has_viewed_playlist_ && changed.playlistId() == viewed_playlist_id_) {
            msgOut_->RequestPlaylistSongs(viewed_playlist_id_, 25);
        }
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_RESPONSE_ADD_SONG_TO_PLAYLIST: {
        const nw::remote::ResponseAddSongToPlaylist response = msg->responseAddSongToPlaylist();
        if (!response.accepted()) {
            ShowMessage("Failed to add song to playlist");
        }
        // On success, the server's PLAYLIST_CHANGED broadcast (for the
        // target playlist) will refresh the view if we happen to be looking
        // at it; no direct action needed here otherwise.
        break;
    }
    case nw::remote::MsgTypeGadget::MsgType::MSG_TYPE_RESPONSE_REMOVE_SONG_FROM_PLAYLIST: {
        const nw::remote::ResponseRemoveSongFromPlaylist response = msg->responseRemoveSongFromPlaylist();
        if (!response.accepted()) {
            ShowMessage("Failed to remove song from playlist");
        }
        break;
    }
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

void Controller::PlaylistTabSelected(int index)
{
    if (index < 0 || index >= playlist_ids_.size()) return;

    viewed_playlist_id_ = playlist_ids_.at(index);
    has_viewed_playlist_ = true;

    // Switching what we're browsing - local play history belongs to whatever
    // we were previously watching, not the newly selected playlist.
    previous_rows_.clear();
    current_row_ = QueueRowData();
    has_current_row_ = false;
    upcoming_rows_.clear();
    current_column_headers_.clear();
    UpdateQueueDisplay();

    msgOut_->RequestPlaylistSongs(viewed_playlist_id_, 25);
}

void Controller::SongDoubleClicked(quint32 row_index)
{
    if (!has_viewed_playlist_) return;
    msgOut_->RequestPlaySong(viewed_playlist_id_, row_index);
}

void Controller::AddCurrentSongToPlaylist(quint32 target_playlist_id, QString new_playlist_name)
{
    msgOut_->RequestAddSongToPlaylist(target_playlist_id, new_playlist_name);
}

void Controller::RemoveSongFromPlaylist(quint32 row_index)
{
    if (!has_viewed_playlist_) return;
    msgOut_->RequestRemoveSongFromPlaylist(viewed_playlist_id_, row_index);
}