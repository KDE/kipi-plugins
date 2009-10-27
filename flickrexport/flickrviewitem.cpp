/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-12-01
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2004 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "flickrviewitem.h"

// Qt includes

#include <QPainter>
#include <QPixmap>

namespace KIPIFlickrExportPlugin
{

GAlbumViewItem::GAlbumViewItem(Q3ListView* parent, const QString& name, const GAlbum& album)
              : Q3ListViewItem(parent, name)
{
    m_album = album;
}

GAlbumViewItem::GAlbumViewItem(Q3ListViewItem* parent, const QString& name, const GAlbum& album)
              : Q3ListViewItem(parent, name)
{
    m_album = album;
}

GAlbumViewItem::~GAlbumViewItem()
{
}

void GAlbumViewItem::paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int)
{
    if (!p)
        return;

    Q3ListView *lv = listView();
    if (!lv)
        return;

    QFontMetrics fm(p->fontMetrics());

    if (isSelected())
        p->fillRect(0, 0, width, height(), cg.color(QColorGroup::Highlight));
    else
        p->fillRect(0, 0, width, height(), cg.color(QColorGroup::Base));

    const QPixmap* icon = pixmap(column);

    int iconWidth = 0;
    if (icon)
    {
        iconWidth = icon->width() + lv->itemMargin();
        int xo    = lv->itemMargin();
        int yo    = (height() - icon->height())/2;
        p->drawPixmap( xo, yo, *icon );
    }

    if (isSelected())
        p->setPen( cg.color(QColorGroup::HighlightedText) );
    else
        p->setPen( cg.color(QColorGroup::Text) );

    int r = lv->itemMargin() + iconWidth;
    int h = lv->fontMetrics().height() + 2;
    p->drawText(r, 0, width-r, h, Qt::AlignVCenter, m_album.title);

    QFont fn(lv->font());
    fn.setPointSize(fn.pointSize()-2);
    fn.setItalic(true);
    p->setFont(fn);
    p->setPen(isSelected() ? cg.color(QColorGroup::HighlightedText) : Qt::gray);
    p->drawText(r, h, width-r, h, Qt::AlignVCenter, m_album.name);
}

void GAlbumViewItem::setup()
{
    int h      = listView()->fontMetrics().height();
    int margin = 4;
    setHeight( qMax(2*h + margin, 32) );
}

void GAlbumViewItem::paintFocus(QPainter*, const QColorGroup&, const QRect&)
{
}

} // namespace KIPIFlickrExportPlugin
