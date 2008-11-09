/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 * Originally based off Kipi plugins code, by Gilles Caulier               *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qevent.h>
#include <q3dragobject.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <q3simplerichtext.h>
//Added by qt3to4:
#include <Q3StrList>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPaintEvent>

#include <klocale.h>

#include "imagelist.h"

using namespace IpodExport;

/////////////////////////////////////////////////////////////////////////////////////////////

ImageList::ImageList( ListType type, QWidget *parent, const char *name )
    : K3ListView( parent, name )
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
        setSorting( -1 );
        setSelectionMode( Q3ListView::Single );
    }

    setItemMargin( 3 );
    setResizeMode( Q3ListView::LastColumn );
    setAllColumnsShowFocus( true );
}

void
ImageList::viewportPaintEvent( QPaintEvent *e )
{
    if( e ) K3ListView::viewportPaintEvent( e );

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
                        "An album needs to be created before images "
                        "can be transferred to the iPod."
                    "</div>" ) );
        }
        Q3SimpleRichText t( minimumText, QApplication::font() );

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

void ImageList::dragEnterEvent( QDragEnterEvent *e )
{
    e->accept( Q3UriDrag::canDecode(e) );
}


bool ImageList::acceptDrag( QDropEvent* e ) const
{
    return Q3UriDrag::canDecode( e );
}

void ImageList::contentsDropEvent( QDropEvent *e )
{
    droppedImagesItems( e );
}

void ImageList::dropEvent( QDropEvent *e )
{
    droppedImagesItems( e );
}

void ImageList::droppedImagesItems( QDropEvent *e )
{
    Q3StrList strList;
    QStringList filesPath;

    if ( !Q3UriDrag::decode(e, strList) ) return;

    Q3StrList stringList;
    Q3StrListIterator it(strList);
    char *str;

    while ( (str = it.current()) != 0 )
    {
        QString filePath = Q3UriDrag::uriToLocalFile(str);
        QFileInfo fileInfo(filePath);

        if( fileInfo.isFile() && fileInfo.exists() )
            filesPath.append( fileInfo.filePath() );

        ++it;
    }

    if( !filesPath.isEmpty() )
       emit addedDropItems( filesPath );
}
