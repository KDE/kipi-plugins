/* ============================================================
 * File  : galleryviewitem.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-03
 * Description :
 *
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
 *
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <QPainter>
#include <QPixmap>

#include "galleryviewitem.h"
#include "gallerytalker.h"

namespace KIPIGalleryExportPlugin
{

GAlbumViewItem::GAlbumViewItem() : QListWidgetItem(0, 1000)
{};

GAlbumViewItem::GAlbumViewItem(QListWidget* parent, const QString& name, const GAlbum& _album)
        : QListWidgetItem(parent/*, name*/) ,
        album(_album)
{};

GAlbumViewItem::GAlbumViewItem(QListWidgetItem* parent, const QString& name, const GAlbum& _album)
        : QListWidgetItem(/*parent,*/name) , album(_album)
{};


void GAlbumViewItem::paintCell(QPainter * p, const QPalette& cg, int column, int width)
{
    if (!p)
        return;

    QListWidget *lv = new QListWidget();
    if (!lv)
        return;
    QFontMetrics fm(p->fontMetrics());

    if (isSelected())
        p->fillRect(0, 0, width, 10 /*height()*/, cg.highlight());
    else
        p->fillRect(0, 0, width, 10 /*height()*/, cg.base());

    const QPixmap * icon = new QPixmap(); //pixmap( column );

    int iconWidth = 0;
    if (icon) {
        iconWidth = icon->width() ;//+ lv->itemMargin();
        int xo    = 10 ;//lv->itemMargin();
        int yo    = (10/*height() - icon->height()*/) / 2;
        p->drawPixmap(xo, yo, *icon);
    }

    if (isSelected())
        p->setPen(cg.highlight().color());
    else
        p->setPen(cg.color(QPalette::Text));

    int r = 10/*lv->itemMargin()*/ + iconWidth;
    int h = lv->fontMetrics().height() + 2;

    // Gallery2 does not return the "name" of the album, instead it
    // returns a reference number than means nothing to the user.
    // We display things slightly differently depending on version.
    if (GalleryTalker::isGallery2()) {
        p->drawText(r, h / 2, width - r, h, Qt::AlignVCenter, album.title);
    } else {
        p->drawText(r, 0, width - r, h, Qt::AlignVCenter, album.title);

        QFont fn(lv->font());
        fn.setPointSize(fn.pointSize() - 2);
        fn.setItalic(true);
        p->setFont(fn);
        p->setPen(isSelected() ? cg.highlight().color() : Qt::gray);
        p->drawText(r, h, width - r, h, Qt::AlignVCenter, album.name);
    }
}

// FIXME, CODE ME!!
void GAlbumViewItem::paintFocus(QPainter* p, const QPalette& cg, const QRect& rc)
{
    return;
};

void GAlbumViewItem::setup()
{
// FIXME
//    int h = listView()->fontMetrics().height();
    int margin = 4;
//    setHeight( qMax(2*h + margin, 32) );
}

}
