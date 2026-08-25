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

#ifndef TOKENPROMPT_H
#define TOKENPROMPT_H

#include <QWidget>

namespace Ui {
class TokenPrompt;
}

class TokenPrompt : public QWidget
{
    Q_OBJECT

public:
    explicit TokenPrompt(QWidget *parent = nullptr);
    ~TokenPrompt();

public slots:
    // Token field + Submit / Bypass / Cancel. Entry point for every auth
    // prompt: initial connect, mid-session AuthStatusChanged(true), and
    // returned to after an invalid-token attempt.
    void ShowEntry();

    // Server said the submitted token was wrong. Message + OK only.
    // OK returns to ShowEntry() so the user can retry.
    void ShowInvalid();

    // Server disconnected us for too many failed attempts. Message + OK
    // only. OK quits the application (handled by Controller).
    void ShowLockout();

private slots:
    void Submit();
    void Bypass();
    void Cancel();
    void Ok();

signals:
    void TokenSubmitted(QString token);
    void BypassRequested();
    void CancelRequested();
    void LockoutAcknowledged();

private:
    enum class Mode { Entry, Invalid, Lockout };

    Ui::TokenPrompt *ui_;
    Mode mode_ = Mode::Entry;
};

#endif // TOKENPROMPT_H