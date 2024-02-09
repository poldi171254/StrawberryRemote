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
    void DisplayText(QString);
    void Finish();

signals:
    void statusChanged(QString status);

private:
    Ui::ConnectionStatus *ui_;
};

#endif // CONNECTIONSTATUS_H
