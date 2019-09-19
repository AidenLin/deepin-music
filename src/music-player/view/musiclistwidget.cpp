/*
 * Copyright (C) 2016 ~ 2018 Wuhan Deepin Technology Co., Ltd.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "musiclistwidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QFocusEvent>

#include <DListWidget>
#include <DLabel>
#include <DPushButton>
#include <DFloatingButton>

#include <DUtil>

#include "../core/playlist.h"
#include "widget/musiclistview.h"
#include "widget/musiclistviewitem.h"
#include "musiclistdatawidget.h"

MusicListWidget::MusicListWidget(QWidget *parent) : DWidget(parent)
{
    setObjectName("MusicListWidget");

    auto layout = new QHBoxLayout(this);
    setFocusPolicy(Qt::ClickFocus);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto musicLayout = new QVBoxLayout(this);
    musicLayout->setContentsMargins(0, 0, 0, 0);
    musicLayout->setSpacing(0);

    DLabel *dataBaseLabel = new DLabel;
    dataBaseLabel->setFixedHeight(40);
    dataBaseLabel->setText(tr("DataBase"));
    dataBaseLabel->setObjectName("MusicListWidgetDataBase");
    dataBaseLabel->setMargin(8);

    DLabel *customizeLabel = new DLabel;
    customizeLabel->setFixedHeight(40);
    customizeLabel->setText(tr("Play List"));
    customizeLabel->setObjectName("MusicListWidgetCustomizeLabel");
    customizeLabel->setMargin(8);

    auto *addListBtn = new DFloatingButton("+");
    addListBtn->setFixedSize(28, 28);
    addListBtn->setFocusPolicy(Qt::NoFocus);

    auto customizeLayout = new QHBoxLayout(this);
    customizeLayout->addWidget(customizeLabel, 100, Qt::AlignLeft);
    customizeLayout->addStretch();
    customizeLayout->addWidget(addListBtn, 0, Qt::AlignRight);

    m_dataBaseListview = new MusicListView;
    m_dataBaseListview->setFixedHeight(162);
    m_customizeListview = new MusicListView;
    m_dataListView = new MusicListDataWidget;

    musicLayout->addWidget(dataBaseLabel, 0, Qt::AlignVCenter);
    musicLayout->addWidget(m_dataBaseListview, 0, Qt::AlignTop);
    musicLayout->addLayout(customizeLayout);
    musicLayout->addWidget(m_customizeListview);

    layout->addLayout(musicLayout, 0);
    layout->addWidget(m_dataListView, 100);

    connect(addListBtn, &DPushButton::clicked, this, [ = ](bool /*checked*/) {
        qDebug() << "addPlaylist(true);";
        Q_EMIT this->addPlaylist(true);
    });

    connect(m_dataBaseListview, &MusicListView::itemClicked,
    this, [ = ](QListWidgetItem * item) {
        this->setEnabled(false);
        auto playlistItem = dynamic_cast<MusicListViewItem *>(item);
        if (!playlistItem) {
            qCritical() << "playlistItem is empty" << item << playlistItem;
            return;
        }

        m_customizeListview->clearSelection();
        m_dataListView->onMusiclistChanged(playlistItem->data());
//        Q_EMIT this->selectPlaylist(playlistItem->data());
        DUtil::TimerSingleShot(500, [this]() {
            this->setEnabled(true);
            Q_EMIT this->hidePlaylist();
        });
    });
    connect(m_dataBaseListview, &MusicListView::currentItemChanged,
    this, [ = ](QListWidgetItem * current, QListWidgetItem * previous) {
        auto itemWidget = dynamic_cast<MusicListViewItem *>(previous);
        itemWidget = dynamic_cast<MusicListViewItem *>(current);
        if (itemWidget) {
            m_dataListView->onMusiclistChanged(itemWidget->data());
        }
    });
    connect(m_dataBaseListview, &MusicListView::customResort,
    this, [ = ](const QStringList & uuids) {
        Q_EMIT this->customResort(uuids);
    });
    connect(m_dataBaseListview, &MusicListView::playall,
    this, [ = ](PlaylistPtr playlist) {
        if (playlist->id() == AlbumMusicListID || playlist->id() == ArtistMusicListID) {
            playlist->playMusicTypeToMeta();
            playlist->play(playlist->first());
        }
        Q_EMIT this->playall(playlist);
    });

    connect(m_customizeListview, &MusicListView::itemClicked,
    this, [ = ](QListWidgetItem * item) {
        this->setEnabled(false);
        auto playlistItem = dynamic_cast<MusicListViewItem *>(item);
        if (!playlistItem) {
            qCritical() << "playlistItem is empty" << item << playlistItem;
            return;
        }

        m_dataBaseListview->clearSelection();
        m_dataListView->onMusiclistChanged(playlistItem->data());
//        Q_EMIT this->selectPlaylist(playlistItem->data());
        DUtil::TimerSingleShot(500, [this]() {
            this->setEnabled(true);
            Q_EMIT this->hidePlaylist();
        });
    });
    connect(m_customizeListview, &MusicListView::currentItemChanged,
    this, [ = ](QListWidgetItem * current, QListWidgetItem * previous) {
        auto itemWidget = dynamic_cast<MusicListViewItem *>(current);
        if (itemWidget) {
            m_dataListView->onMusiclistChanged(itemWidget->data());
        }
    });
    connect(m_customizeListview, &MusicListView::customResort,
    this, [ = ](const QStringList & uuids) {
        Q_EMIT this->customResort(uuids);
    });
    connect(m_customizeListview, &MusicListView::playall,
    this, [ = ](PlaylistPtr playlist) {
        Q_EMIT this->playall(playlist);
    });

    //musiclistdatawidget
    connect(m_dataListView, &MusicListDataWidget::playall,
    this, [ = ](PlaylistPtr playlist) {
//        Q_EMIT this->selectPlaylist(playlist);
        Q_EMIT this->playall(playlist);
    });
    connect(m_dataListView, &MusicListDataWidget::playMedia,
    this, [ = ](PlaylistPtr playlist, const MetaPtr meta) {
        Q_EMIT this->playMedia(playlist, meta);
    });
    connect(m_dataListView, &MusicListDataWidget::resort,
    this, [ = ](PlaylistPtr playlist, int sortType) {
        Q_EMIT this->resort(playlist, sortType);
    });
    connect(m_dataListView, &MusicListDataWidget::requestCustomContextMenu,
    this, [ = ](const QPoint & pos, char type) {
        Q_EMIT this->requestCustomContextMenu(pos, type);
    });
    connect(m_dataListView, &MusicListDataWidget::addToPlaylist,
    this, [ = ](PlaylistPtr playlist, const MetaPtrList  metalist) {
        Q_EMIT this->addToPlaylist(playlist, metalist);
    });
    connect(m_dataListView, &MusicListDataWidget::musiclistRemove,
    this, [ = ](PlaylistPtr playlist, const MetaPtrList  & metalist) {
        Q_EMIT this->musiclistRemove(playlist, metalist);
    });
    connect(m_dataListView, &MusicListDataWidget::musiclistDelete,
    this, [ = ](PlaylistPtr playlist, const MetaPtrList  & metalist) {
        Q_EMIT this->musiclistDelete(playlist, metalist);
    });
    connect(m_dataListView, &MusicListDataWidget::showInfoDialog,
    this, [ = ](const MetaPtr meta) {
        Q_EMIT this->showInfoDialog(meta);
    });
    connect(m_dataListView, &MusicListDataWidget::updateMetaCodec,
    this, [ = ](const MetaPtr  meta) {
        Q_EMIT this->updateMetaCodec(meta);
    });
}

void MusicListWidget::initData(QList<PlaylistPtr > playlists, PlaylistPtr last)
{
    if (playlists.length() <= 0) {
        qCritical() << "playlist is empty";
        return;
    }

    QListWidgetItem *currentDataBaseItem = nullptr;
    QListWidgetItem *currentcustomizeItem = nullptr;
    for (auto &playlist : playlists) {
        if (playlist->hide()) {
            continue;
        }

        qDebug() << "init with" << playlist->id() << playlist->displayName();

        auto item = new MusicListViewItem(playlist);
        if (playlist->id() == AlbumMusicListID || playlist->id() == ArtistMusicListID ||
                playlist->id() == AllMusicListID || playlist->id() == FavMusicListID ) {
            m_dataBaseListview->addItem(item);
            if (playlist->playing())
                currentDataBaseItem = item;
        } else {
            m_customizeListview->addItem(item);
            currentcustomizeItem = item;
        }

    }

    if (currentDataBaseItem) {
        m_dataBaseListview->setCurrentItem(currentDataBaseItem);
    }
    if (currentcustomizeItem) {
        m_dataBaseListview->setCurrentItem(currentcustomizeItem);
    }
    if (currentDataBaseItem == nullptr && currentcustomizeItem == nullptr) {
        m_dataBaseListview->setCurrentItem(m_dataBaseListview->item(0));
    }
}

void MusicListWidget::onMusicPlayed(PlaylistPtr playlist, const MetaPtr)
{
    for (int i = 0; i < m_dataBaseListview->count(); ++i) {
        QListWidgetItem *item = m_dataBaseListview->item(i);
        auto playlistItem = dynamic_cast<MusicListViewItem *>(item);
    }
}

void MusicListWidget::focusOutEvent(QFocusEvent *event)
{
    // TODO: monitor mouse position
    QPoint mousePos = mapToParent(mapFromGlobal(QCursor::pos()));
    if (!this->geometry().contains(mousePos)) {
        if (event && event->reason() == Qt::MouseFocusReason) {
            DUtil::TimerSingleShot(50, [this]() {
                qDebug() << "self lost focus hide";
                Q_EMIT this->hidePlaylist();
            });
        }
    }
    DWidget::focusOutEvent(event);
}

void MusicListWidget::onPlaylistAdded(PlaylistPtr playlist)
{
    if (playlist->hide()) {
        return;
    }

    auto item = new MusicListViewItem(playlist);
    if (playlist->id() == AlbumMusicListID || playlist->id() == ArtistMusicListID ||
            playlist->id() == AllMusicListID || playlist->id() == FavMusicListID ) {
        m_dataBaseListview->addItem(item);
    } else {
        m_customizeListview->addItem(item);
    }
    if (playlist->playing())
        m_dataListView->onMusiclistChanged(playlist);
}

void MusicListWidget::onCurrentChanged(PlaylistPtr playlist)
{
    if (playlist) {
        m_dataBaseListview->clearSelection();
        m_customizeListview->clearSelection();
    }
    for (int i = 0; i < m_dataBaseListview->count(); ++i) {
        QListWidgetItem *item = m_dataBaseListview->item(i);
        auto playlistItem = dynamic_cast<MusicListViewItem *>(item);
        if (playlist == playlistItem->data())
            m_dataBaseListview->setCurrentItem(item);
    }
    for (int i = 0; i < m_customizeListview->count(); ++i) {
        QListWidgetItem *item = m_customizeListview->item(i);
        auto playlistItem = dynamic_cast<MusicListViewItem *>(item);
        if (playlist == playlistItem->data())
            m_customizeListview->setCurrentItem(item);
    }
}

void MusicListWidget::onMusiclistChanged(PlaylistPtr playlist)
{
    m_dataListView->onMusiclistChanged(playlist);
}

void MusicListWidget::onMusicListAdded(PlaylistPtr playlist, const MetaPtrList metalist)
{
    Q_UNUSED(playlist)
    Q_UNUSED(metalist)
    m_dataListView->onMusiclistUpdate();
}

void MusicListWidget::onMusicListRemoved(PlaylistPtr playlist, const MetaPtrList metalist)
{
    Q_UNUSED(playlist)
    Q_UNUSED(metalist)
    m_dataListView->onMusiclistUpdate();
}

void MusicListWidget::onMusiclistUpdate()
{
    m_dataListView->onMusiclistUpdate();
}

void MusicListWidget::onCustomContextMenuRequest(const QPoint &pos, PlaylistPtr selectedlist, PlaylistPtr favlist, QList<PlaylistPtr> newlists)
{
    m_dataListView->onCustomContextMenuRequest(pos, selectedlist, favlist, newlists);
}

