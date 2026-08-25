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


#include "connectionstatus.h"
#include "ui_connectionstatus.h"

class Controller;

ConnectionStatus::ConnectionStatus(QWidget *parent)
    : QWidget(parent),
      ui_(new Ui::ConnectionStatus)
{
    ui_->setupUi(this);
    ui_->connectionStatus->setReadOnly(true);
    QWidget::connect(ui_->continueButton, &QAbstractButton::pressed, this, &ConnectionStatus::Connect);
    QWidget::connect(ui_->cancelButton, &QAbstractButton::pressed, this, &ConnectionStatus::Finish);
}

ConnectionStatus::~ConnectionStatus()
{
    delete ui_;
}

void ConnectionStatus::DisplayText(QString text)
{
    ui_->connectionStatus->setText(text);
}

void ConnectionStatus::Connect()
{
    emit Continue();
}

void ConnectionStatus::Finish()
{
    emit Cancel();
}


