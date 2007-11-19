/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Include files for Qt

#include <qevent.h>
#include <qdragobject.h>
#include <qstrlist.h>
#include <qfileinfo.h>
#include <qwhatsthis.h>
#include <qlistview.h>

// Include files for KDE

#include <klocale.h>

// Local include files

#include "batchprocessimageslist.h"
#include "batchprocessimageslist.moc"

namespace KIPIBatchProcessImagesPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////

BatchProcessImagesList::BatchProcessImagesList(QWidget *parent, const char *name)
                      : KListView(parent, name)
{
    setAcceptDrops(true);
    setDropVisualizer(false);
    addColumn(i18n("Source Album"));
    addColumn(i18n("Source Image"));
    addColumn(i18n("Target Image"));
    addColumn(i18n("Result"));
    setSorting(3);
    setItemMargin(3);
    setResizeMode(QListView::LastColumn);
    setSelectionMode(QListView::Single);
    setAllColumnsShowFocus ( true );
    QWhatsThis::add( this, i18n("<p>You can see here the operations' results "
                                "during the process. Double-click on an item for more "
                                "information once the process has ended. "
                                "<p>You can use the \"Add\" button or drag-and-drop "
                                "to add some new items to the list. "
                                "<p>If the items are taken from different Albums "
                                "the process' results will be merged to the target Album.") );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesList::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(QUriDrag::canDecode(e));
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool BatchProcessImagesList::acceptDrag(QDropEvent* e) const
{
    return ( QUriDrag::canDecode(e) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesList::contentsDropEvent(QDropEvent* e)
{
    droppedImagesItems(e);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesList::dropEvent(QDropEvent *e)
{
    droppedImagesItems(e);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesList::droppedImagesItems(QDropEvent *e)
{
    QStrList strList;
    QStringList FilesPath;

    if ( !QUriDrag::decode(e, strList) ) return;

    QStrList stringList;
    QStrListIterator it(strList);
    char *str;

    while ( (str = it.current()) != 0 )
       {
       QString filePath = QUriDrag::uriToLocalFile(str);
       QFileInfo fileInfo(filePath);

       if (fileInfo.isFile() && fileInfo.exists())
          FilesPath.append(fileInfo.filePath());

       ++it;
       }

    if (FilesPath.isEmpty() == false)
       emit addedDropItems(FilesPath);
}

}  // NameSpace KIPIBatchProcessImagesPlugin
