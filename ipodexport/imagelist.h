//////////////////////////////////////////////////////////////////////////////
//
//    ImageList.H
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
//              (C) 2006 Seb Ruiz <me@sebruiz.net>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <klistview.h>

namespace IpodExport
{

class ImageList : public KListView
{
    Q_OBJECT

    public:
        enum ListType { UploadType, IpodType };

        ImageList( ListType=UploadType, QWidget *parent=0, const char *name=0 );

        ListType getType() const { return m_type; }

    signals:
        void addedDropItems( QStringList filesPath );

    protected:
        bool acceptDrag( QDropEvent *e ) const;
        void contentsDropEvent( QDropEvent *e );
        void dragEnterEvent( QDragEnterEvent *e );
        void dropEvent( QDropEvent *e );
        void droppedImagesItems( QDropEvent *e );
        void viewportPaintEvent( QPaintEvent *e );

    private:
        ListType m_type;
};

}

#endif
