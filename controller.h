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

private:
    void MsgHandler();
    void ShowMessage(const QString &text);

    // Remaining-time countdown. The server sends the current position and the
    // track length with the song info; between those updates the client ticks
    // down locally once per second while the player is playing.
    void TickRemaining();
    void UpdateRemainingDisplay();
    void UpdateQueueDisplay();

    static constexpr int kMaxPreviousRows = 50;

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
};

#endif // CONTROLLER_H