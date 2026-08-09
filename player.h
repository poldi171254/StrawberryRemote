#ifndef PLAYER_H
#define PLAYER_H

#include <QDialog>
#include <QStringList>
#include <QList>
#include <QPoint>

namespace Ui {
class Player;
}

struct QueueRowData {
    QStringList values;
    quint32 row_index = 0;
};

// One visible column's identity - name plus whether the server classifies it
// as numeric/measurement-like (Track, Year, Length, PlayCount, etc.) vs. free
// text (Title, Artist, Album, Genre, etc.). Numeric columns are centered.
struct ColumnInfo {
    QString name;
    bool is_numeric = false;

    bool operator==(const ColumnInfo &other) const {
        return name == other.name && is_numeric == other.is_numeric;
    }
    bool operator!=(const ColumnInfo &other) const {
        return !(*this == other);
    }
};

class Player : public QDialog
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();
    void SetRemaining(QString);
    void SetMessage(QString);
    void SetPlaylists(const QStringList &names, const QList<quint32> &ids, int active_index);
    // columns: visible column identities, left to right.
    // previous_rows: capped local history, oldest first - not double-click-able
    // (their row indices may be stale by now).
    // current_row / has_current_row: the row currently playing, if any.
    // upcoming_rows: songs after the current row.
    // Each row carries its absolute playlist row index, used for double-click-to-play
    // and for the "Remove from playlist" context menu action.
    void SetQueue(const QList<ColumnInfo> &columns,
                  const QList<QueueRowData> &previous_rows,
                  const QueueRowData &current_row,
                  bool has_current_row,
                  const QList<QueueRowData> &upcoming_rows);

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
    void PlaylistTabSelected(int index);
    void SongDoubleClicked(quint32 row_index);
    // target_playlist_id is ignored by the server when new_playlist_name is non-empty.
    void AddCurrentSongToPlaylist(quint32 target_playlist_id, QString new_playlist_name);
    void RemoveSongFromPlaylist(quint32 row_index);

private slots:
    void ShowQueueContextMenu(const QPoint &pos);

private:
    Ui::Player *ui_;
    QList<quint32> playlist_ids_;  // parallel to playlistTabBar's tabs
};

#endif // PLAYER_H