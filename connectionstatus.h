/*
 * Strawberry Music Player Client
 * Copyright 2026, Leopold List <leo@zudiewiener.com>
 *
 * The client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 */

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
