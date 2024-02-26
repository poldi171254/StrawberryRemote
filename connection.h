#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "outgoingmsg.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);
    ~Connection();

public slots:
    void Init(QString, int);
    bool Connect();

private slots:

    void Error(QAbstractSocket::SocketError);

signals:
    void ConnectionError();

private:
    QString ipAddr_;
    int port_;
    QTcpSocket *socket_;
    QHostAddress hostAddr_;
    OutgoingMsg *msgOut_;
    bool statusOk = false;

};

#endif // CONNECTION_H
