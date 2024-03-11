#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::Player)
{
    ui_->setupUi(this);

    //ui_->title->setEnabled(false);

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

}

void Player::SetArtist(QString artist)
{
    ui_->artist->setPlaceholderText(artist);
}

void Player::SetAlbum(QString album)
{
    ui_->album->setPlaceholderText(album);
}

void Player::SetTrack(QString track)
{
    ui_->track->setPlaceholderText(track);
}

void Player::SetYear(QString year)
{
    ui_->year->setPlaceholderText(year);
}

void Player::SetGenre(QString genre)
{
    ui_->genre->setPlaceholderText(genre);
}

void Player::SetPlayCount(QString playCount)
{
    ui_->playcount->setPlaceholderText(playCount);
}

void Player::SetSongLength(QString songLength)
{
    ui_->songLength->setPlaceholderText(songLength);
}

void Player::SetMessage(QString statusMsg)
{
    ui_->statusMessage->setPlaceholderText(statusMsg);
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



