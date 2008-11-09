/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "ipodlistitem.h"

#include <klocale.h>

using namespace IpodExport;

////////////////////////////////////////////
/// Class IpodAlbumItem
////////////////////////////////////////////

IpodAlbumItem::IpodAlbumItem( QListView *parent, QListViewItem *after, Itdb_PhotoAlbum *pa )
    : KListViewItem( parent, after )
    , m_photoAlbum( pa )
{
    // don't use setName, as it writes to the ipod
    m_name = pa->name;
    if( m_name.isEmpty() )
        m_name = i18n( "Unnamed" );
    setText( 0, m_name );
}

void IpodAlbumItem::setPhotoAlbum( Itdb_PhotoAlbum *pa )
{
    m_photoAlbum = pa;
}

void IpodAlbumItem::setName( const QString & name )
{
    if( name == m_name )
        return;

    if( m_photoAlbum )
        strcpy( m_photoAlbum->name, name.utf8() );

    m_name = name;
    setText( 0, m_name );
}


////////////////////////////////////////////
/// Class IpodPhotoItem
////////////////////////////////////////////

IpodPhotoItem::IpodPhotoItem( IpodAlbumItem *parent, IpodPhotoItem *after,
                              Itdb_Artwork *art )
    : KListViewItem( parent, after )
    , m_artwork( art )
{
}

void IpodPhotoItem::setArtwork( Itdb_Artwork *art )
{
    m_artwork = art;
}


