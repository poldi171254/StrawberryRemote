#ifndef OUTGOINGMSG_H
#define OUTGOINGMSG_H

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>
#include <QDebug>
#include "proto/RemoteMessages.pb.h"


class OutgoingMsg : public QObject
{
    Q_OBJECT
public:
    explicit OutgoingMsg(QObject *parent = nullptr);
    ~OutgoingMsg();
    bool Start(QTcpSocket*);

signals:

private:
    void Send();
    QTcpSocket *socket_;
    bool statusOk_ = false;
    nw::remote::Message *msg_;

    long bytesOut_;
};

#endif // OUTGOINGMSG_H
