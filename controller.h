#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
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

private:
    void MsgHandler();
    void ShowMessage(const QString &text);

    // Remaining-time countdown. The server sends the current position and the
    // track length with the song info; between those updates the client ticks
    // down locally once per second while the player is playing.
    void TickRemaining();
    void UpdateRemainingDisplay();

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
};

#endif // CONTROLLER_H