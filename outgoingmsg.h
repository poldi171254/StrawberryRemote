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
    void RequestAddSongToPlaylist(quint32 target_playlist_id, QString new_playlist_name);
    void RequestRemoveSongFromPlaylist(quint32 playlist_id, quint32 row_index);

private:

    QTcpSocket *socket_;
    bool statusOk_ = false;
    nw::remote::Message msg_;
    long bytesOut_;
};

#endif // OUTGOINGMSG_H