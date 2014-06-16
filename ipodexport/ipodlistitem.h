/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2009 by Seb Ruiz <ruiz at kde dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef IPODLISTITEM_H
#define IPODLISTITEM_H

// Qt includes

#include <QTreeWidget>
#include <QTreeWidgetItem>

// libgpod includes

extern "C"
{
#include <gpod/itdb.h>
}

namespace KIPIIpodExportPlugin
{

class IpodAlbumItem : public QTreeWidgetItem
{
public:

    IpodAlbumItem(QTreeWidget* const parent, Itdb_PhotoAlbum* const pa);

    QString          name()       const { return m_name;       }
    Itdb_PhotoAlbum* photoAlbum() const { return m_photoAlbum; }

    void setPhotoAlbum(Itdb_PhotoAlbum* const pa);
    void setName(const QString& name);

private:

    QString          m_name;
    Itdb_PhotoAlbum* m_photoAlbum;
};

// ----------------------------------------------------------------------------

class IpodPhotoItem : public QTreeWidgetItem
{
public:

    IpodPhotoItem(IpodAlbumItem* const parent, IpodPhotoItem* const prev, Itdb_Artwork* const art);

    Itdb_Artwork* artwork() const { return m_artwork; }
    void          setArtwork(Itdb_Artwork* const art);

private:

    Itdb_Artwork* m_artwork;
};

} // namespace KIPIIpodExportPlugin

#endif  // IPODLISTITEM_H
