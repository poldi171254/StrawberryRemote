#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::Player)
{
    ui_->setupUi(this);
}

Player::~Player()
{
    delete ui_;
}
