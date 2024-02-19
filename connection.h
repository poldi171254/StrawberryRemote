#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "spb/remote/RemoteMessages.qpb.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);
    ~Connection();

public slots:
    void Init(QString, int);
    void Connect();

private slots:

    void Error(QAbstractSocket::SocketError);

signals:
    void ConnectionError();

private:
    QString ipAddr_;
    int port_;
    QTcpSocket *socket_;
    QHostAddress hostAddr_;
    spb::remote::Message *msg_;

};

#endif // CONNECTION_H
