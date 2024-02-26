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
#include "messagewindow.h"


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
    void DoWork();
    void DisplayError(QString);

signals:
    void Finish();

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
    MessageWindow *errorWindow_;


};

#endif // CONTROLLER_H
