#ifndef OUTGOINGMSG_H
#define OUTGOINGMSG_H

#include <QObject>

#include "spb/remote/RemoteMessages.qpb.h"

class OutgoingMsg : public QObject
{
    Q_OBJECT
public:
    explicit OutgoingMsg(QObject *parent = nullptr);
    ~OutgoingMsg();
    void Start();
    void Send();

signals:

private:
    spb::remote::Message *message_;
};

#endif // OUTGOINGMSG_H
