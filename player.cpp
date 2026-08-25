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


#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
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
    QWidget::connect(ui_->playlistTabBar, &QTabBar::currentChanged, this, &Player::PlaylistTabSelected);

    QWidget::connect(ui_->queueTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        Q_UNUSED(column);
        QTableWidgetItem *item = ui_->queueTable->item(row, 0);
        if (!item) return;
        const QVariant row_index_data = item->data(Qt::UserRole);
        if (!row_index_data.isValid()) return;  // history row - no eligible index attached
        emit SongDoubleClicked(row_index_data.toUInt());
    });

    ui_->queueTable->setContextMenuPolicy(Qt::CustomContextMenu);
    QWidget::connect(ui_->queueTable, &QTableWidget::customContextMenuRequested, this, &Player::ShowQueueContextMenu);
}

Player::~Player()
{
    delete ui_;
}

void Player::SetRemaining(QString remaining)
{
    ui_->songlengthremaining->setText(remaining);
}

void Player::SetMessage(QString statusMsg)
{
    ui_->statusMessage->setPlaceholderText(statusMsg);
}

void Player::SetPlaylists(const QStringList &names, const QList<quint32> &ids, int active_index)
{
    playlist_ids_ = ids;

    ui_->playlistTabBar->blockSignals(true);
    while (ui_->playlistTabBar->count() > 0) {
        ui_->playlistTabBar->removeTab(0);
    }
    for (const QString &name : names) {
        ui_->playlistTabBar->addTab(name);
    }
    if (active_index >= 0 && active_index < ui_->playlistTabBar->count()) {
        ui_->playlistTabBar->setCurrentIndex(active_index);
    }
    ui_->playlistTabBar->blockSignals(false);
}

void Player::SetQueue(const QList<ColumnInfo> &columns,
                      const QList<QueueRowData> &previous_rows,
                      const QueueRowData &current_row,
                      bool has_current_row,
                      const QList<QueueRowData> &upcoming_rows)
{
    ui_->queueTable->clear();
    ui_->queueTable->setColumnCount(columns.size());

    for (int col = 0; col < columns.size(); ++col) {
        QTableWidgetItem *header_item = new QTableWidgetItem(columns.at(col).name);
        header_item->setTextAlignment(
            columns.at(col).is_numeric ? (Qt::AlignCenter) : (Qt::AlignLeft | Qt::AlignVCenter));
        ui_->queueTable->setHorizontalHeaderItem(col, header_item);
    }

    const int total_rows = static_cast<int>(previous_rows.size())
                           + (has_current_row ? 1 : 0)
                           + static_cast<int>(upcoming_rows.size());
    ui_->queueTable->setRowCount(total_rows);

    int table_row = 0;

    auto fill_row = [&](const QueueRowData &row_data, bool is_current, bool is_previous, bool attach_row_index) {
        for (int col = 0; col < columns.size(); ++col) {
            const QString value = (col < row_data.values.size()) ? row_data.values.at(col) : QString();
            QTableWidgetItem *item = new QTableWidgetItem(value);
            item->setTextAlignment(
                columns.at(col).is_numeric ? (Qt::AlignCenter) : (Qt::AlignLeft | Qt::AlignVCenter));
            if (is_current) {
                QFont f = item->font();
                f.setBold(true);
                item->setFont(f);
            }
            else if (is_previous) {
                item->setForeground(Qt::gray);
            }
            if (col == 0 && attach_row_index) {
                item->setData(Qt::UserRole, row_data.row_index);
                item->setData(Qt::UserRole + 1, is_current);
            }
            ui_->queueTable->setItem(table_row, col, item);
        }
        ++table_row;
    };

    // Previous/history rows deliberately get no row_index attached - their
    // indices may be stale by now, so double-click / context menu on them is a no-op.
    for (const QueueRowData &row : previous_rows) {
        fill_row(row, false, true, false);
    }
    if (has_current_row) {
        fill_row(current_row, true, false, true);
    }
    for (const QueueRowData &row : upcoming_rows) {
        fill_row(row, false, false, true);
    }
}

void Player::SetPlaylistsMutable(bool mutable_allowed)
{
    playlists_mutable_ = mutable_allowed;
}

void Player::ShowQueueContextMenu(const QPoint &pos)
{
    QTableWidgetItem *clicked_item = ui_->queueTable->itemAt(pos);
    if (!clicked_item) return;

    const int row = clicked_item->row();
    QTableWidgetItem *first_col_item = ui_->queueTable->item(row, 0);
    if (!first_col_item) return;

    const QVariant row_index_data = first_col_item->data(Qt::UserRole);
    if (!row_index_data.isValid()) return;  // history row - nothing actionable

    if (!playlists_mutable_) {
        QMessageBox::information(this, tr("Token Required"),
                                 tr("Token verification is required for this feature."));
        return;
    }

    const bool is_current = first_col_item->data(Qt::UserRole + 1).toBool();
    const quint32 row_index = row_index_data.toUInt();

    QMenu menu(this);

    // "Add to another playlist" only makes sense on the current row: the server
    // always adds whatever is actually playing (Player::GetCurrentItem()), not
    // an arbitrary clicked row, so offering this on upcoming rows would be misleading.
    if (is_current) {
        QMenu *add_menu = menu.addMenu(tr("Add to another playlist"));
        const int current_tab_index = ui_->playlistTabBar->currentIndex();
        const quint32 exclude_id = (current_tab_index >= 0 && current_tab_index < playlist_ids_.size())
                                       ? playlist_ids_.at(current_tab_index) : 0;

        for (int i = 0; i < playlist_ids_.size(); ++i) {
            const quint32 target_id = playlist_ids_.at(i);
            if (target_id == exclude_id) continue;
            QAction *existing_action = add_menu->addAction(ui_->playlistTabBar->tabText(i));
            QWidget::connect(existing_action, &QAction::triggered, this, [this, target_id]() {
                emit AddCurrentSongToPlaylist(target_id, QString());
            });
        }

        add_menu->addSeparator();
        QAction *new_playlist_action = add_menu->addAction(tr("New playlist..."));
        QWidget::connect(new_playlist_action, &QAction::triggered, this, [this]() {
            bool ok = false;
            const QString name = QInputDialog::getText(this, tr("New Playlist"), tr("Playlist name:"), QLineEdit::Normal, QString(), &ok);
            if (ok && !name.isEmpty()) {
                emit AddCurrentSongToPlaylist(0, name);
            }
        });
    }

    QAction *remove_action = menu.addAction(tr("Remove from playlist"));
    QWidget::connect(remove_action, &QAction::triggered, this, [this, row_index]() {
        emit RemoveSongFromPlaylist(row_index);
    });

    menu.exec(ui_->queueTable->viewport()->mapToGlobal(pos));
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