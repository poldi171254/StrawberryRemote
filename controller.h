#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>


#include "connectionstatus.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();
    void Init(QString, qint16);
    QString GetIpAddress();

signals:
    void Finish();

private slots:
    void Ready();
    void Error(QAbstractSocket::SocketError);

private:
    ConnectionStatus *statusWindow_;
    QString ipAddr_ = "0.0.0.0";
    qint16 port_;
    QHostAddress hostAddr_;
    //QTcpServer *server_;
    QTcpSocket *socket_;


};

#endif // CONTROLLER_H
