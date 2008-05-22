/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
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

// Qt includes.

#include <qpainter.h>
#include <qpixmap.h>

// Local includes.

#include "picasawebviewitem.h"

namespace KIPIPicasawebExportPlugin
{

void GAlbumViewItem::paintCell(QPainter* p, const QColorGroup& cg,
                               int column, int width, int )
{
    if (!p)
        return;

    QListView *lv = listView();
    if (!lv)
        return;

    QFontMetrics fm(p->fontMetrics());

    if (isSelected())
        p->fillRect(0, 0, width, height(), cg.highlight());
    else
        p->fillRect(0, 0, width, height(), cg.base());

    const QPixmap * icon = pixmap( column );

    int iconWidth = 0;
    if (icon)
    {
        iconWidth = icon->width() + lv->itemMargin();
        int xo    = lv->itemMargin();
        int yo    = (height() - icon->height())/2;
        p->drawPixmap( xo, yo, *icon );
    }

    if (isSelected())
        p->setPen( cg.highlightedText() );
    else
        p->setPen( cg.text() );

    int r = lv->itemMargin() + iconWidth;
    int h = lv->fontMetrics().height() + 2;
    p->drawText(r, 0, width-r, h, Qt::AlignVCenter, album.title);

    QFont fn(lv->font());
    fn.setPointSize(fn.pointSize()-2);
    fn.setItalic(true);
    p->setFont(fn);
    p->setPen(isSelected() ? cg.highlightedText() : Qt::gray);
    p->drawText(r, h, width-r, h, Qt::AlignVCenter, album.name);
}

void GAlbumViewItem::setup()
{
    int h      = listView()->fontMetrics().height();
    int margin = 4;
    setHeight(QMAX(2*h + margin, 32));
}

} // namespace KIPIPicasawebExportPlugin
