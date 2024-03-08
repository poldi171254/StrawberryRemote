#ifndef INCOMINGMSG_H
#define INCOMINGMSG_H

#include <QObject>

#include <google/protobuf/message.h>

#include "proto/RemoteMessages.pb.h"
#include "qtcpsocket.h"

class IncomingMsg : public QObject
{
    Q_OBJECT
public:
    explicit IncomingMsg(QObject *parent = nullptr);
    ~IncomingMsg();
    void Start(QTcpSocket*);
    void ProcessMsg();
    nw::remote::Message* GetMsg();


private slots:
    void ReadyRead();


signals:
    void InMsgParsed();

private:
    nw::remote::Message *msg_;
    QTcpSocket *socket_;
    long bytesIn_;
    QByteArray msgStream_;
    std::string msgString_;
    qint32 msgType_;
};

#endif // INCOMINGMSG_H
