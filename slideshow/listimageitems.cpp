/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-21
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2003-2007 Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT includes

#include <qstring.h>
#include <qwidget.h>
#include <qevent.h>
#include <q3strlist.h>
#include <q3dragobject.h>
#include <qfileinfo.h>
#include <q3listbox.h>
//Added by qt3to4:
#include <QDropEvent>
#include <QDragEnterEvent>

// KDE includes

#include <k3listbox.h>
#include <kurl.h>

// Local includes

#include "listimageitems.h"
#include "listimageitems.moc"

namespace KIPISlideShowPlugin
{

ListImageItems::ListImageItems(QWidget *parent, const char *name)
              : K3ListBox(parent, name)
{
    setSelectionMode (Q3ListBox::Extended);
    setAcceptDrops(true);
}

void ListImageItems::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(Q3UriDrag::canDecode(e));
}

void ListImageItems::dropEvent(QDropEvent *e)
{
    Q3StrList strList;
    KUrl::List filesUrl;

    if ( !Q3UriDrag::decode(e, strList) ) return;

    Q3StrList stringList;
    QStrListIterator it(strList);
    char *str;

    while ( (str = it.current()) != 0 )
    {
        QString filePath = Q3UriDrag::uriToLocalFile(str);
        QFileInfo fileInfo(filePath);

        if (fileInfo.isFile() && fileInfo.exists())
        {
            KUrl url(fileInfo.filePath());
            filesUrl.append(url);
        }

        ++it;
    }

    if (filesUrl.isEmpty() == false)
        emit addedDropItems(filesUrl);
}

}  // NameSpace KIPISlideShowPlugin
