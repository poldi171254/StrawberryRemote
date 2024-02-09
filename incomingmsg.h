#ifndef INCOMINGMSG_H
#define INCOMINGMSG_H

#include <QObject>

#include "spb/remote/RemoteMessages.qpb.h"

class IncomingMsg : public QObject
{
    Q_OBJECT
public:
    explicit IncomingMsg(QObject *parent = nullptr);

signals:
};

#endif // INCOMINGMSG_H
