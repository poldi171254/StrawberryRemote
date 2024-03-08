#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "messagewindow.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);
    ~Connection();
    void DisplayError();
    QTcpSocket *GetSocket();


public slots:
    bool Init(QString, int);
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
    MessageWindow *msgWindow_;
    QString errorMsg_;
    bool statusOk_;

};

#endif // CONNECTION_H
