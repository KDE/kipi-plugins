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

#include <KLocale>

#include "ImageList.h"

namespace KIPIIpodExportPlugin
{

ImageList::ImageList( ListType type, QWidget *parent )
    : QTreeWidget( parent )
    , m_type( type )
{
    if( type == ImageList::UploadType )
    {
        setColumnCount( 2 );
        setAcceptDrops( true );
        //addColumn( i18n("Image") );
    }
    else if( type == ImageList::IpodType )
    {
        //addColumn( i18n("Albums") );
        setRootIsDecorated( true ); // show expand icons
        setSortingEnabled( false );
    }
}

// TODO: Port to KDE4

/*
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
*/

} // namespace KIPIIpodExportPlugin
