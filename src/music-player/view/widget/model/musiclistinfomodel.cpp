/*
 * Copyright (C) 2017 ~ 2018 Wuhan Deepin Technology Co., Ltd.
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

#include "musiclistinfomodel.h"

#include <QDebug>
#include "core/medialibrary.h"

class MusiclistInfomodelPrivate
{
public:
    MusiclistInfomodelPrivate(MusiclistInfomodel *parent) : q_ptr(parent) {}

    PlaylistPtr playlist;

    MusiclistInfomodel *q_ptr;
    Q_DECLARE_PUBLIC(MusiclistInfomodel)
};

MusiclistInfomodel::MusiclistInfomodel(QObject *parent) :
    QStandardItemModel(parent), d_ptr(new MusiclistInfomodelPrivate(this))
{

}

MusiclistInfomodel::MusiclistInfomodel(int rows, int columns, QObject *parent):
    QStandardItemModel(rows, columns, parent), d_ptr(new MusiclistInfomodelPrivate(this))
{

}

MusiclistInfomodel::~MusiclistInfomodel()
{

}

PlaylistPtr MusiclistInfomodel::playlist() const
{
    Q_D(const MusiclistInfomodel);
    return d->playlist;
}

void MusiclistInfomodel::setPlaylist(PlaylistPtr playlist)
{
    Q_D(MusiclistInfomodel);
    d->playlist = playlist;
}

QModelIndex MusiclistInfomodel::findIndex(const MetaPtr meta)
{
    if (meta.isNull())
        return QModelIndex();
    Q_ASSERT(!meta.isNull());
    Q_D(MusiclistInfomodel);

    if (!d->playlist.isNull()) {
        auto row = d->playlist->index(meta->hash);
        auto itemIndex = this->index(row, 0);

        auto itemHash = data(itemIndex).toString();
        if (itemHash == meta->hash) {
            return itemIndex;
        }

        //   qCritical() << "search index failed" << meta->hash << itemHash;

        for (int i = 0; i < rowCount(); ++i) {
            itemIndex = index(i, 0);
            auto itemHash = data(itemIndex).toString();
            if (itemHash == meta->hash) {
                return itemIndex;
            }
        }
    }

    return QModelIndex();
}

bool MusiclistInfomodel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return QStandardItemModel::setData(index, value, role);
}

MetaPtr MusiclistInfomodel::meta(const QModelIndex &index, int role) const
{
    auto hash = this->data(index, role).toString();
    return MediaLibrary::instance()->meta(hash);
}

//#include <QMimeData>

//bool MusiclistInfomodel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
//{
//    if (action == Qt::IgnoreAction)
//        return true;

//    if (!data->hasFormat("text/uri-list"))
//        return false;

//    if (column > 0)
//        return false;

//    int beginRow;

//    if (row != -1)
//        beginRow = row;
//    else if (parent.isValid())
//        beginRow = parent.row();
//}

Qt::DropActions MusiclistInfomodel::supportedDropActions() const
{
//    return QAbstractItemModel::supportedDropActions();
    return /*Qt::CopyAction | */Qt::MoveAction;
}

Qt::ItemFlags MusiclistInfomodel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | defaultFlags;
    } else {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}
