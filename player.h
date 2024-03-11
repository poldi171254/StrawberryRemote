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
    void SetTitle(QString);
    void SetArtist(QString);
    void SetAlbum(QString);
    void SetTrack(QString);
    void SetYear(QString);
    void SetGenre(QString);
    void SetPlayCount(QString);
    void SetSongLength(QString);
    void SetMessage(QString);

public slots:
    void Play();
    void Pause();
    void Next();
    void Previous();
    void Finish();

signals:
    void Playing();
    void Paused();
    void PlayNext();
    void PlayPrevisous();
    void Finished();

private:
    Ui::Player *ui_;
};

#endif // PLAYER_H
