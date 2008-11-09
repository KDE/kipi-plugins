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

#ifndef IPODLISTITEM_H
#define IPODLISTITEM_H

extern "C" {
#include <gpod/itdb.h>
}

#include <klistview.h>

namespace IpodExport
{

    class IpodAlbumItem : public KListViewItem
    {
        public:
            IpodAlbumItem( Q3ListView *parent, Q3ListViewItem *after, Itdb_PhotoAlbum *pa );

            QString          name()       const { return m_name;       }
            Itdb_PhotoAlbum *photoAlbum() const { return m_photoAlbum; }

            void setPhotoAlbum( Itdb_PhotoAlbum *pa );
            void setName( const QString & name );

        private:
            QString          m_name;
            Itdb_PhotoAlbum *m_photoAlbum;
    };

    class IpodPhotoItem : public KListViewItem
    {
        public:
            IpodPhotoItem( IpodAlbumItem *parent, IpodPhotoItem *after, Itdb_Artwork *art );

            Itdb_Artwork *artwork() const { return m_artwork; }
            void          setArtwork( Itdb_Artwork *art );

        private:
            Itdb_Artwork *m_artwork;
    };

}

#endif  // IPODLISTITEM_H
