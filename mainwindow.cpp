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


#include <QDebug>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      controller_(new Controller)
{
    ui_->setupUi(this);
    //controller_ = new Controller();
    QWidget::connect(ui_->connectButton, &QAbstractButton::pressed, this, &MainWindow::connect);
    QWidget::connect(ui_->finishButton, &QAbstractButton::pressed, this, &MainWindow::Finish);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::Init()
{
    ipAddress_ = controller_->GetIpAddress();
    ui_->ipAddress->setText(ipAddress_);
}

void MainWindow::Finish()
{
    QMainWindow::close();
}

void MainWindow::connect()
{
    ipAddress_ = ui_->ipAddress->text();
    port_ = ui_->port->value();
    ui_->connectButton->setEnabled(false);
    controller_->Init(ipAddress_, port_);
    Finish();
}
