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


#include "messagewindow.h"
#include "ui_messagewindow.h"

MessageWindow::MessageWindow(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::MessageWindow)
{
    ui_->setupUi(this);
    QWidget::connect(ui_->okButton, &QAbstractButton::pressed, this, &MessageWindow::Ok);
}

MessageWindow::~MessageWindow()
{
    delete ui_;
}

void MessageWindow::Init()
{
}

void MessageWindow::DisplayText(QString message)
{
    ui_->messageText->setText(message);
}
