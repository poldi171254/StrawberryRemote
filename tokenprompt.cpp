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

#include "tokenprompt.h"
#include "ui_tokenprompt.h"

TokenPrompt::TokenPrompt(QWidget *parent)
    : QWidget(parent),
    ui_(new Ui::TokenPrompt)
{
    ui_->setupUi(this);
    setWindowFlag(Qt::Window, true);
    setWindowModality(Qt::ApplicationModal);

    QObject::connect(ui_->submitButton, &QPushButton::clicked, this, &TokenPrompt::Submit);
    QObject::connect(ui_->bypassButton, &QPushButton::clicked, this, &TokenPrompt::Bypass);
    QObject::connect(ui_->cancelButton, &QPushButton::clicked, this, &TokenPrompt::Cancel);
    QObject::connect(ui_->okButton, &QPushButton::clicked, this, &TokenPrompt::Ok);
}

TokenPrompt::~TokenPrompt()
{
    delete ui_;
}

void TokenPrompt::ShowEntry()
{
    mode_ = Mode::Entry;
    ui_->errorLabel->clear();
    ui_->tokenEdit->clear();

    ui_->tokenEdit->setVisible(true);
    ui_->submitButton->setVisible(true);
    ui_->bypassButton->setVisible(true);
    ui_->cancelButton->setVisible(true);
    ui_->okButton->setVisible(false);

    ui_->tokenEdit->setFocus();
}

void TokenPrompt::ShowInvalid()
{
    mode_ = Mode::Invalid;
    ui_->errorLabel->setText("Incorrect token");
    ui_->tokenEdit->clear();

    ui_->tokenEdit->setVisible(false);
    ui_->submitButton->setVisible(false);
    ui_->bypassButton->setVisible(false);
    ui_->cancelButton->setVisible(false);
    ui_->okButton->setVisible(true);

    ui_->okButton->setFocus();
}

void TokenPrompt::ShowLockout()
{
    mode_ = Mode::Lockout;
    ui_->errorLabel->setText("Too many failed attempts - the server has closed the connection.");
    ui_->tokenEdit->clear();

    ui_->tokenEdit->setVisible(false);
    ui_->submitButton->setVisible(false);
    ui_->bypassButton->setVisible(false);
    ui_->cancelButton->setVisible(false);
    ui_->okButton->setVisible(true);

    ui_->okButton->setFocus();
}

void TokenPrompt::Submit()
{
    emit TokenSubmitted(ui_->tokenEdit->text());
}

void TokenPrompt::Bypass()
{
    emit BypassRequested();
}

void TokenPrompt::Cancel()
{
    emit CancelRequested();
}

void TokenPrompt::Ok()
{
    // ShowInvalid()'s OK returns to entry so the user can retry; ShowLockout()'s
    // OK needs to quit the app instead.
    if (mode_ == Mode::Lockout) {
        emit LockoutAcknowledged();
    }
    else {
        ShowEntry();
    }
}