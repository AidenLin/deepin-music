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

#pragma once

#include <QScopedPointer>
#include "DMainWindow"
#include "core/playlist.h"

class Presenter;
class MainFramePrivate;
class MainFrame : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainFrame(QWidget *parent = Q_NULLPTR);
    ~MainFrame() Q_DECL_OVERRIDE;

    void initUI(bool showLoading);
    void postInitUI();
    void binding(Presenter *presenter);

    void focusPlayList();
    QString coverBackground() const;

signals:
    void requitQuit();
    void addPlaylist(bool);
    void importSelectFiles(const QStringList &filelist, PlaylistPtr playlist);
    void triggerShortcutAction(const QString &optKey);
    void fadeInOut();
    void exit();

public slots:
    void updateUI();
    void setCoverBackground(QString coverBackground);
    void onSelectImportDirectory();
    void onSelectImportFiles();
    void slotTheme(int type);
    void changePicture();
    void onViewShortcut();
protected:
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    QScopedPointer<MainFramePrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), MainFrame)
};

extern const QString s_PropertyViewname;
extern const QString s_PropertyViewnameLyric;

