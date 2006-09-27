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

#ifndef IMAGELISTITEM_H
#define IMAGELISTITEM_H

extern "C" {
#include <gpod/itdb.h>
}

#include <qstring.h>
#include <klistview.h>

namespace IpodExport
{

    class ImageListItem : public KListViewItem
    {
    public:

        /// for use by the upload widget
        ImageListItem( QListView *parent, QString const & pathSrc, QString const & name )
            : KListViewItem( parent, QString::null/*set below*/, name )
            , m_pathSrc( pathSrc )
            , m_name( name )
            , m_photoAlbum( 0 )
        {
            setText( 0, pathSrc.section('/', -2, -2) );
        }

        /// for use by the ipod widget
        ImageListItem( QListView *parent, QListViewItem *after, Itdb_PhotoAlbum * pa )
            : KListViewItem( parent, after )
            , m_pathSrc( QString::null )
            , m_name( QString::null )
            , m_photoAlbum( pa )
        {
            setText( 0, pa->name );
        }

        QString          pathSrc()    const { return m_pathSrc;    }
        QString          name()       const { return m_name;       }
        Itdb_PhotoAlbum *photoAlbum() const { return m_photoAlbum; }

        void setPhotoAlbum( Itdb_PhotoAlbum *pa ) { m_photoAlbum = pa; }
        void setName( const QString & name )
        {
            if( m_photoAlbum )
                strcpy( m_photoAlbum->name, name.utf8() );

            m_name = m_photoAlbum->name;
            setText( 0, m_photoAlbum->name );
        }

    private:
        QString          m_pathSrc;
        QString          m_name;
        Itdb_PhotoAlbum *m_photoAlbum;
    };

}

#endif  // IMAGELISTITEM_H
