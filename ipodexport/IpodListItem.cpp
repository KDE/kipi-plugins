/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz@kde.org>
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

#include "IpodListItem.h"

#include <klocale.h>

namespace KIPIIpodExportPlugin
{

IpodAlbumItem::IpodAlbumItem(Q3ListView *parent, Q3ListViewItem *after, Itdb_PhotoAlbum *pa)
             : K3ListViewItem(parent, after), m_photoAlbum(pa)
{
    // don't use setName, as it writes to the ipod
    m_name = pa->name;
    if( m_name.isEmpty() )
        m_name = i18n( "Unnamed" );
    setText( 0, m_name );
}

void IpodAlbumItem::setPhotoAlbum(Itdb_PhotoAlbum *pa)
{
    m_photoAlbum = pa;
}

void IpodAlbumItem::setName(const QString& name)
{
    if( name == m_name )
        return;

    if( m_photoAlbum )
        strcpy( m_photoAlbum->name, name.utf8() );

    m_name = name;
    setText( 0, m_name );
}

// ---------------------------------------------------------------------------

IpodPhotoItem::IpodPhotoItem(IpodAlbumItem *parent, IpodPhotoItem *after,
                             Itdb_Artwork *art)
    : K3ListViewItem(parent, after), m_artwork(art)
{
}

void IpodPhotoItem::setArtwork( Itdb_Artwork *art )
{
    m_artwork = art;
}

} // namespace KIPIIpodExportPlugin
