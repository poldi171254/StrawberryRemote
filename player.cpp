#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::Player)
{
    ui_->setupUi(this);

    QWidget::connect(ui_->playButton, &QAbstractButton::pressed, this, &Player::Play);
    QWidget::connect(ui_->pauseButton, &QAbstractButton::pressed, this, &Player::Pause);
    QWidget::connect(ui_->nextButton, &QAbstractButton::pressed, this, &Player::Next);
    QWidget::connect(ui_->previouseButton, &QAbstractButton::pressed, this, &Player::Previous);
    QWidget::connect(ui_->finishButton, &QAbstractButton::pressed, this, &Player::Finish);
}

Player::~Player()
{
    delete ui_;
}

void Player::SetTitle(QString title)
{
    ui_->title->setPlaceholderText(title);
    ui_->title->setEnabled(false);
}

void Player::SetMessage(QString StatusMsg)
{
    ui_->statusMessage->setPlaceholderText(StatusMsg);
}

void Player::Play()
{
    emit Playing();
}

void Player::Pause()
{
    emit Paused();
}

void Player::Next()
{
    emit PlayNext();
}

void Player::Previous()
{
    emit PlayPrevisous();
}

void Player::Finish()
{
    emit Finished();
}



