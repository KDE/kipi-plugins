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

#include "ipodlistitem.h"

// KDE includes

#include <kicon.h>
#include <klocale.h>

namespace KIPIIpodExportPlugin
{

IpodAlbumItem::IpodAlbumItem(QTreeWidget* const parent, Itdb_PhotoAlbum* const pa )
    : QTreeWidgetItem(parent), m_photoAlbum(pa)
{
    // don't use setName, as it writes to the ipod
    m_name = pa->name;

    if( m_name.isEmpty() )
        m_name = i18n( "Unnamed" );

    setText( 0, m_name );
    setIcon( 0, KIcon("folder") );
}

void IpodAlbumItem::setPhotoAlbum(Itdb_PhotoAlbum* const pa)
{
    m_photoAlbum = pa;
}

void IpodAlbumItem::setName(const QString& name)
{
    if( name == m_name )
        return;

    if( m_photoAlbum )
        strcpy( m_photoAlbum->name, name.toUtf8() );

    m_name = name;
    setText( 0, m_name );
}

// ---------------------------------------------------------------------------

IpodPhotoItem::IpodPhotoItem(IpodAlbumItem* const parent, IpodPhotoItem* const prev, Itdb_Artwork* const art)
    : QTreeWidgetItem(parent, prev), m_artwork(art)
{
    setIcon( 0, KIcon("image-jp2") );
}

void IpodPhotoItem::setArtwork(Itdb_Artwork* const art)
{
    m_artwork = art;
}

} // namespace KIPIIpodExportPlugin
