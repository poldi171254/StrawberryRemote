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

#ifndef OUTGOINGMSG_H
#define OUTGOINGMSG_H

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>
#include <QDebug>
#include "RemoteMessages.qpb.h"


class OutgoingMsg : public QObject
{
    Q_OBJECT
public:
    explicit OutgoingMsg(QObject *parent = nullptr);
    ~OutgoingMsg();
    void Start(QTcpSocket*);
    void RequestSongInfo();
    void RequestPlay();
    void RequestPause();
    void RequestPrevious();
    void RequestNext();
    void RequestFinish();
    void RequestInitialInfo();
    void Send(nw::remote::MsgTypeGadget::MsgType msg_type);
    void RequestPlaylistSongs(quint32 playlist_id, quint32 upcoming_count);
    void RequestPlaySong(quint32 playlist_id, quint32 row_index);
    void RequestAddSongToPlaylist(quint32 target_playlist_id, QString new_playlist_name, QString token);
    void RequestRemoveSongFromPlaylist(quint32 playlist_id, quint32 row_index, QString token);
    void RequestValidateToken(QString token);


private:

    QTcpSocket *socket_;
    bool statusOk_ = false;
    nw::remote::Message msg_;
    long bytesOut_;
};

#endif // OUTGOINGMSG_H