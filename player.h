#ifndef PLAYER_H
#define PLAYER_H

#include <QDialog>
#include <QStringList>
#include <QList>
#include <QPoint>
#include <QEvent>
#include <QElapsedTimer>

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

    // Controls whether right-clicking a song offers the playlist-mutating
    // context menu (Add to another playlist / Remove from playlist) or shows
    // an explanatory message instead. Reflects Controller's current
    // mutable-playlist state (auth not required, valid token, or bypassed).
    void SetPlaylistsMutable(bool mutable_allowed);

    // Seek slider. length_seconds is the song length; the slider range becomes
    // 0..length-1 and the slider is enabled only if `enabled` and length > 0.
    void SetSeekRange(quint32 length_seconds, bool enabled);
    // Elapsed seconds, pushed by the controller. Ignored while the handle is held.
    void SetSeekPosition(quint32 elapsed_seconds);

    // Volume slider (server-controlled). The row is hidden unless the server
    // says volume control is enabled.
    void SetVolumeControlEnabled(bool enabled);
    // 0..100, from the server. Ignored while the handle is held and for a short
    // time after release (echoes of our own requests). Never emits VolumeRequested.
    void SetVolume(quint32 volume);

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
    // Emitted once, on release, only if the handle ended somewhere other than
    // where the controller last put it.
    void SeekRequested(quint32 position_seconds);
    // Throttled to about one per 100 ms while dragging; always emitted on release.
    void VolumeRequested(quint32 volume);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void ShowQueueContextMenu(const QPoint &pos);
    void ShowDragRemaining(int value);

private:
    Ui::Player *ui_;
    QList<quint32> playlist_ids_;  // parallel to playlistTabBar's tabs
    bool playlists_mutable_ = true;
    quint32 seek_length_ = 0;
    int seek_reference_ = 0;  // last position pushed by the controller
    QElapsedTimer volume_send_timer_;     // time since the last volume request while dragging
    QElapsedTimer volume_release_timer_;  // time since the volume handle was released
};

#endif // PLAYER_H