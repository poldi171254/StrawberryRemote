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


#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QTimer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>

#include "connection.h"
#include "connectionstatus.h"
#include "incomingmsg.h"
#include "outgoingmsg.h"
#include "player.h"
#include "tokenprompt.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    void Init(QString ipAddr, int port);
    QString GetIpAddress();

public slots:
    void Cancel();
    void Continue();
    void IncomingMsgReceived();
    void Play();
    void Pause();
    void Next();
    void Previous();
    void Finish();
    void Ready();
    void ConnectionError();
    void SocketDisconnected();
    void PlaylistTabSelected(int index);
    void SongDoubleClicked(quint32 row_index);
    void AddCurrentSongToPlaylist(quint32 target_playlist_id, QString new_playlist_name);
    void RemoveSongFromPlaylist(quint32 row_index);

    // TokenPrompt signal handlers
    void TokenSubmitted(QString token);
    void BypassRequested();
    void TokenPromptCancelled();
    void LockoutAcknowledged();

private:
    void MsgHandler();
    void ShowMessage(const QString &text);

    // Remaining-time countdown. The server sends the current position and the
    // track length with the song info; between those updates the client ticks
    // down locally once per second while the player is playing.
    void TickRemaining();
    void UpdateRemainingDisplay();
    void UpdateQueueDisplay();

    // Puts the client into "waiting on the user's auth decision" state:
    // hides player_ (if shown) and shows tokenPrompt_ in entry mode. Used by
    // both the initial connect and a mid-session AuthStatusChanged(true).
    void BeginAuthPrompt();

    static constexpr int kMaxPreviousRows = 50;

    // Whether the user is currently allowed to add/remove playlist entries.
    // Disabled: waiting on an auth decision, player_ not shown yet.
    // Enabled: auth not required, or a valid token was supplied.
    // Bypassed: user chose to skip authentication; mutating attempts are
    // intercepted client-side with an explanatory message.
    enum class PlaylistMutableState { Disabled, Enabled, Bypassed };

    QList<ColumnInfo> current_column_headers_;
    QList<QueueRowData> previous_rows_;
    QueueRowData current_row_;
    bool has_current_row_ = false;
    QList<QueueRowData> upcoming_rows_;

    QStringList playlist_names_;
    int active_playlist_tab_index_ = -1;

    Connection      *connection_   = nullptr;
    IncomingMsg     *msgIn_        = nullptr;
    OutgoingMsg     *msgOut_       = nullptr;
    Player          *player_       = nullptr;
    ConnectionStatus *statusWindow_ = nullptr;
    TokenPrompt     *tokenPrompt_  = nullptr;
    QTcpSocket      *socket_       = nullptr;

    QTimer *countdown_timer_ = nullptr;
    int remaining_seconds_ = 0;

    QString ipAddr_;
    int port_ = 0;
    bool statusOk_ = false;
    bool expecting_disconnect_ = false;
    quint32 active_playlist_id_ = 0;
    bool has_active_playlist_ = false;
    QList<quint32> playlist_ids_;        // parallel to playlist_names_, indexed the same
    quint32 viewed_playlist_id_ = 0;
    bool has_viewed_playlist_ = false;

    QString token_;
    bool auth_enabled_ = false;
    PlaylistMutableState mutable_state_ = PlaylistMutableState::Enabled;
};

#endif // CONTROLLER_H