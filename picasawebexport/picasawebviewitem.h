/* ============================================================
 * File  : picasawebviewitem.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-01
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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
 * ============================================================ */

#ifndef PICASAWEBVIEWITEM_H
#define PICASAWEBVIEWITEM_H

#include <qlistview.h>

#include "picasawebitem.h"

namespace KIPIPicasawebExportPlugin
{

class GAlbumViewItem : public QListViewItem
{
public:

    GAlbumViewItem(QListView* parent, const QString& name,
                   const GAlbum& _album)
        : QListViewItem(parent, name), album(_album) {}
    GAlbumViewItem(QListViewItem* parent, const QString& name,
                   const GAlbum& _album)
        : QListViewItem(parent, name), album(_album) {}

    GAlbum album;

    void paintCell(QPainter * p, const QColorGroup & cg,
                   int column, int width, int );
    void paintFocus (QPainter*, const QColorGroup&,
                     const QRect&) {}

protected:

    void setup();
    
};

}

#endif /* PICASAWEBVIEWITEM_H */
