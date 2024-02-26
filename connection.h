#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "outgoingmsg.h"
#include "messagewindow.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);
    ~Connection();
    void DisplayError();


public slots:
    bool Init(QString, int);
    bool Connect();
    void Close();

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
    MessageWindow *errorWindow_;
    QString errorMsg_;
    bool statusOk_;

};

#endif // CONNECTION_H
