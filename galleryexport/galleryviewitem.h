/* ============================================================
 * File  : galleryviewitem.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-01
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
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

#ifndef GALLERYVIEWITEM_H
#define GALLERYVIEWITEM_H

#include <q3listview.h>

#include "galleryitem.h"

namespace KIPIGalleryExportPlugin
{

class GAlbumViewItem : public Q3ListViewItem
{
public:

    GAlbumViewItem(Q3ListView* parent, const QString& name,
                   const GAlbum& _album)
        : Q3ListViewItem(parent, name), album(_album) {}
    GAlbumViewItem(Q3ListViewItem* parent, const QString& name,
                   const GAlbum& _album)
        : Q3ListViewItem(parent, name), album(_album) {}

    GAlbum album;

    void paintCell(QPainter * p, const QColorGroup & cg,
                   int column, int width, int );
    void paintFocus (QPainter*, const QColorGroup&,
                     const QRect&) {}

protected:

    void setup();
    
};

}

#endif /* GALLERYVIEWITEM_H */
