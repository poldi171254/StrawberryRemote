#ifndef PLAYER_H
#define PLAYER_H

#include <QDialog>

namespace Ui {
class Player;
}

class Player : public QDialog
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();

private:
    Ui::Player *ui_;
};

#endif // PLAYER_H
