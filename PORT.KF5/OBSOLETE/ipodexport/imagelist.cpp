/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2009 by Seb Ruiz <ruiz at kde dot org>
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

#include "imagelist.moc"

// Qt includes

#include <QObject>
#include <QHeaderView>
#include <QMimeData>
#include <QUrl>

// KDE includes

#include <klocale.h>

namespace KIPIIpodExportPlugin
{

ImageList::ImageList(ListType type, QWidget* const parent)
    : QTreeWidget(parent), m_type(type)
{
    if( type == ImageList::UploadType )
    {
        setAcceptDrops( true );
        //addColumn( i18n("Image") );
    }
    else if( type == ImageList::IpodType )
    {
        //addColumn( i18n("Albums") );
        setRootIsDecorated( true ); // show expand icons
        setSortingEnabled( false );
    }

    // Remove the header
    header()->hide();
}

bool ImageList::dropMimeData(QTreeWidgetItem* /*parent*/, int /*index*/, const QMimeData *data, Qt::DropAction /*action*/)
{
    droppedImagesItems(data->urls());
    return true;
}

void ImageList::droppedImagesItems(const QList<QUrl>& urls)
{
    QStringList filesPath;

    Q_FOREACH(const QUrl& u, urls)
    {
        filesPath << u.path();
    }

    if( !filesPath.isEmpty() )
    {
       Q_EMIT signalAddedDropItems(filesPath);
    }
}

} // namespace KIPIIpodExportPlugin
