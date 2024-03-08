#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include "connection.h"
#include "player.h"
#include "incomingmsg.h"
#include "outgoingmsg.h"


#include "connectionstatus.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();
    void Init(QString, int);
    QString GetIpAddress();
    void Cancel();
    void Continue();
    void MsgHandler();
    void IncomingMsgReceived();
    void Play();
    void Pause();
    void Next();
    void Previous();
    void Finish();

signals:
    //void Finish();

private slots:
    void Ready();
    void ConnectionError();

private:
    ConnectionStatus *statusWindow_;
    QString ipAddr_;
    int port_;
    Connection *connection_;
    Player *player_;
    bool statusOk_ = false;
    OutgoingMsg *msgOut_;
    IncomingMsg *msgIn_;
    QTcpSocket *socket_;
};

#endif // CONTROLLER_H
