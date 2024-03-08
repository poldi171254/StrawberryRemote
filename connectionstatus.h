#ifndef CONNECTIONSTATUS_H
#define CONNECTIONSTATUS_H

#include <QWidget>

namespace Ui {
class ConnectionStatus;
}

class ConnectionStatus : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionStatus(QWidget *parent = nullptr);
    ~ConnectionStatus();
    bool statusOk();

public slots:
    void DisplayText(QString);
    void Connect();
    void Finish();

signals:
    void statusChanged(QString status);
    void Continue();
    void Cancel();

private:
    Ui::ConnectionStatus *ui_;
    bool statusOk_ = false;
};

#endif // CONNECTIONSTATUS_H
