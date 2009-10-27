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

#ifndef FLICKRVIEWITEM_H
#define FLICKRVIEWITEM_H

// Qt includes

#include <Q3ListView>

// Local includes

#include "flickritem.h"

namespace KIPIFlickrExportPlugin
{

class GAlbumViewItem : public Q3ListViewItem
{

public:

    GAlbumViewItem(Q3ListView* parent, const QString& name, const GAlbum& album);
    GAlbumViewItem(Q3ListViewItem* parent, const QString& name, const GAlbum& album);
    ~GAlbumViewItem();

protected:

    void paintCell(QPainter *p, const QColorGroup& cg, int column, int width, int);
    void paintFocus(QPainter*, const QColorGroup&, const QRect&);
    void setup();

private:

    GAlbum m_album;
};

} // namespace KIPIFlickrExportPlugin

#endif /* FLICKRVIEWITEM_H */
