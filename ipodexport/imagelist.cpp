//////////////////////////////////////////////////////////////////////////////
//
//    ImageList.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
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

// Include files for Qt

#include <qapplication.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qsimplerichtext.h>

#include <klocale.h>

#include "imagelist.h"

using namespace IpodExport;

/////////////////////////////////////////////////////////////////////////////////////////////

ImageList::ImageList( ListType type, QWidget *parent, const char *name )
    : KListView( parent, name )
    , m_type( type )
{
    if( type == ImageList::UploadType )
    {
        setAcceptDrops( true );
        setDropVisualizer( false );
        addColumn( i18n("Source Album") );
        addColumn( i18n("Image") );
    }
    else if( type == ImageList::IpodType )
    {
        addColumn( i18n("Albums") );
        setRootIsDecorated( true ); // show expand icons
    }

    setSorting( 3 );
    setItemMargin( 3 );
    setResizeMode( QListView::LastColumn );
    setSelectionMode( QListView::Single );
    setAllColumnsShowFocus ( true );

}

void
ImageList::viewportPaintEvent( QPaintEvent *e )
{
    if( e ) KListView::viewportPaintEvent( e );

    if( !childCount() && e )
    {
        QPainter p( viewport() );
        QString minimumText;

        if( m_type == UploadType )
        {
            minimumText = (i18n(
                    "<div align=center>"
                    "<h3>Upload Queue</h3>"
                        "To create a queue, "
                        "<b>drag</b> images and "
                        "<b>drop</b> them here.<br><br>"
                    "</div>" ) );
        }
        else if( m_type == IpodType )
        {
            minimumText = (i18n(
                    "<div align=center>"
                    "<h3>iPod Albums</h3>"
                        "An album needs to be created before images, "
                        "can be transferred to the iPod."
                    "</div>" ) );
        }
        QSimpleRichText t( minimumText, QApplication::font() );

        if ( t.width()+30 >= viewport()->width() || t.height()+30 >= viewport()->height() )
            //too big, giving up
            return;

        const uint w = t.width();
        const uint h = t.height();
        const uint x = (viewport()->width() - w - 30) / 2 ;
        const uint y = (viewport()->height() - h - 30) / 2 ;

        p.setBrush( colorGroup().background() );
        p.drawRoundRect( x, y, w+30, h+30, (8*200)/w, (8*200)/h );
        t.draw( &p, x+15, y+15, QRect(), colorGroup() );
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ImageList::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(QUriDrag::canDecode(e));
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool ImageList::acceptDrag(QDropEvent* e) const
{
    return ( QUriDrag::canDecode(e) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ImageList::contentsDropEvent(QDropEvent* e)
{
    droppedImagesItems(e);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ImageList::dropEvent(QDropEvent *e)
{
    droppedImagesItems(e);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ImageList::droppedImagesItems(QDropEvent *e)
{
    QStrList strList;
    QStringList filesPath;

    if ( !QUriDrag::decode(e, strList) ) return;

    QStrList stringList;
    QStrListIterator it(strList);
    char *str;

    while ( (str = it.current()) != 0 )
    {
        QString filePath = QUriDrag::uriToLocalFile(str);
        QFileInfo fileInfo(filePath);

        if( fileInfo.isFile() && fileInfo.exists() )
            filesPath.append( fileInfo.filePath() );

        ++it;
    }

    if( !filesPath.isEmpty() )
       emit addedDropItems( filesPath );
}
