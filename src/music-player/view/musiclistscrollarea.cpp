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

#include "musiclistscrollarea.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QFocusEvent>

#include <DListWidget>
#include <DLabel>
#include <DPushButton>
#include <DFloatingButton>
#include <DPalette>

#include <DUtil>

#include "../core/playlist.h"
#include "../core/musicsettings.h"
#include "widget/musiclistview.h"
#include "widget/musiclistviewitem.h"
#include "musiclistdatawidget.h"
#include "widget/musicimagebutton.h"
#include <DGuiApplicationHelper>

MusicListScrollArea::MusicListScrollArea(QWidget *parent) : DScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);
    setFixedWidth(220);
    setWidgetResizable(true);

    setAutoFillBackground(true);
    auto leftFramePalette = palette();
    leftFramePalette.setColor(DPalette::Background, QColor("#FFFFFF"));
    setPalette(leftFramePalette);

    QWidget *widget = new QWidget(this);
    setWidget(widget);

    auto musicLayout = new QVBoxLayout(widget);
    musicLayout->setContentsMargins(10, 5, 10, 5);
    musicLayout->setSpacing(0);

    dataBaseLabel = new DLabel;
    dataBaseLabel->setFixedHeight(40);
    dataBaseLabel->setText(tr("Library"));
    dataBaseLabel->setObjectName("MusicListScrollAreaDataBase");
    dataBaseLabel->setMargin(10);
    auto dataBaseLabelFont = dataBaseLabel->font();
    dataBaseLabelFont.setFamily("SourceHanSansSC");
    dataBaseLabelFont.setWeight(QFont::Medium);
    dataBaseLabelFont.setPixelSize(14);
    dataBaseLabel->setFont(dataBaseLabelFont);

    customizeLabel = new DLabel;
    customizeLabel->setFixedHeight(40);
    customizeLabel->setText(tr("Playlists"));
    customizeLabel->setObjectName("MusicListScrollAreaCustomizeLabel");
    customizeLabel->setMargin(10);
    customizeLabel->setFont(dataBaseLabelFont);

    m_addListBtn = new MusicImageButton(":/mpimage/light/normal/add_normal.svg",
                                        ":/mpimage/light/hover/add_hover.svg",
                                        ":/mpimage/light/press/add_press.svg");
    m_addListBtn->setFixedSize(37, 37);
    m_addListBtn->setFocusPolicy(Qt::NoFocus);

    auto customizeLayout = new QHBoxLayout(widget);
    customizeLayout->setContentsMargins(0, 0, 5, 0);
    customizeLayout->addWidget(customizeLabel, 100, Qt::AlignLeft);
    customizeLayout->addStretch();
    customizeLayout->addWidget(m_addListBtn, 0, Qt::AlignBottom);

    m_dataBaseListview = new MusicListView;
    m_dataBaseListview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dataBaseListview->setFixedHeight(162);
    m_customizeListview = new MusicListView;
    musicLayout->setContentsMargins(0, 0, 0, 0);

    musicLayout->addWidget(dataBaseLabel, 0, Qt::AlignVCenter);
    musicLayout->addWidget(m_dataBaseListview, 0, Qt::AlignTop);
    musicLayout->addLayout(customizeLayout);
    musicLayout->addWidget(m_customizeListview);

    int themeType = DGuiApplicationHelper::instance()->themeType();
    slotTheme(themeType);
}

MusicListView *MusicListScrollArea::getDBMusicListView()
{
    return m_dataBaseListview;
}

MusicListView *MusicListScrollArea::getCustomMusicListView()
{
    return m_customizeListview;
}

MusicImageButton *MusicListScrollArea::getAddButton()
{
    return m_addListBtn;
}

void MusicListScrollArea::slotTheme(int type)
{
    if (type == 0)
        type = DGuiApplicationHelper::instance()->themeType();
    if (type == 1) {
        DPalette leftFramePalette = palette();
        leftFramePalette.setColor(DPalette::Background, QColor("#FFFFFF"));
        setPalette(leftFramePalette);

        DPalette dataBaseLabelPalette = dataBaseLabel->palette();
        QColor WindowTextColor("#757F92");
        WindowTextColor.setAlphaF(0.8);
        dataBaseLabelPalette.setColor(DPalette::WindowText, WindowTextColor);
        dataBaseLabel->setPalette(dataBaseLabelPalette);
        customizeLabel->setPalette(dataBaseLabelPalette);

        m_addListBtn->setPropertyPic(":/mpimage/light/normal/add_normal.svg",
                                     ":/mpimage/light/hover/add_hover.svg",
                                     ":/mpimage/light/press/add_press.svg");
    } else {
        auto leftFramePalette = palette();
        leftFramePalette.setColor(DPalette::Background, QColor("#232323"));
        setPalette(leftFramePalette);

        DPalette dataBaseLabelPalette = dataBaseLabel->palette();
        QColor WindowTextColor("#C0C6D4");
        WindowTextColor.setAlphaF(0.8);
        dataBaseLabelPalette.setColor(DPalette::WindowText, WindowTextColor);
        dataBaseLabel->setPalette(dataBaseLabelPalette);
        customizeLabel->setPalette(dataBaseLabelPalette);

        m_addListBtn->setPropertyPic(":/mpimage/dark/normal/add_normal.svg",
                                     ":/mpimage/dark/hover/add_hover.svg",
                                     ":/mpimage/dark/press/add_press.svg");
    }

    m_dataBaseListview->slotTheme(type);
    m_customizeListview->slotTheme(type);
}

void MusicListScrollArea::changePicture(QPixmap pixmap, QPixmap albumPixmap, QPixmap sidebarPixmap)
{
    m_dataBaseListview->changePicture(pixmap, sidebarPixmap);
    m_customizeListview->changePicture(pixmap, sidebarPixmap);
}

void MusicListScrollArea::resizeEvent(QResizeEvent *event)
{
    m_customizeListview->adjustHeight();
    DScrollArea::resizeEvent(event);
    sizeHint();
}

