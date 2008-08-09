/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-24
 * Description : 
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 *
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */



#ifndef GALLERYVIEWITEM_H
#define GALLERYVIEWITEM_H

// Qt includes
#include <QTreeWidget>
#include <QPalette>

// local includes
#include "galleryitem.h"


namespace KIPIGalleryExportPlugin
{

class GAlbumViewItem : public QTreeWidgetItem
{
public:

    GAlbumViewItem();

    GAlbumViewItem(QTreeWidget* parent, const QString& name, const GAlbum& _album);

    GAlbumViewItem(QTreeWidgetItem* parent, const QString& name, const GAlbum& _album);

    GAlbum album;

    void paintCell(QPainter* p, const QPalette& cg, int column, int width);

    void paintFocus(QPainter* p, const QPalette& cg, const QRect& rc);

protected:

    void setup();

};

}

#endif /* GALLERYVIEWITEM_H */
