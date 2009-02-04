/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-21
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

#include "listimageitems.h"
#include "listimageitems.moc"

#define ICONSIZE 32

// Qt includes.

#include <Q3StrList>
#include <Q3DragObject>
#include <QWidget>
#include <QEvent>
#include <QFileInfo>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QIcon>

// KDE includes.

#include <kurl.h>
#include <kiconloader.h>
#include <kdebug.h>


namespace KIPIAdvancedSlideshowPlugin
{

ImageItem::ImageItem(QListWidget* parent, QString const & name, QString const & comments,
                     QString const & path, QString const & album, KIPI::Interface* interface)
        : QListWidgetItem(parent), m_name(name), m_comments(comments), m_path(path), m_album(album)
{
    setIcon(SmallIcon("image-x-generic", ICONSIZE, KIconLoader::DisabledState));

    m_interface = interface;
    connect(m_interface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
            this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));
    
    KUrl url;
    url.setPath(path);
    interface->thumbnail(url, ICONSIZE);
}

ImageItem::~ImageItem()
{
}

QString ImageItem::comments()
{
    return m_comments;
}

QString ImageItem::name()
{
    return m_name;
}

QString ImageItem::path()
{
    return m_path;
}

QString ImageItem::album()
{
    return m_album;
}

void ImageItem::setName(const QString &newName)
{
    setText(newName);
}

void ImageItem::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    if (url!=m_path)
        return;
    
    if (!pix.isNull())
        setIcon(pix.scaled(ICONSIZE, ICONSIZE, Qt::KeepAspectRatio));

    disconnect(m_interface, 0, this, 0);
}

// ---------------------------------------------

ListImageItems::ListImageItems(QWidget *parent)
        : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    setSortingEnabled(false);
    setIconSize(QSize(ICONSIZE, ICONSIZE));
}

void ListImageItems::dragEnterEvent(QDragEnterEvent *e)
{
    e->setAccepted(Q3UriDrag::canDecode(e));
}

void ListImageItems::dropEvent(QDropEvent *e)
{
    Q3StrList strList;
    KUrl::List filesUrl;

    if ( !Q3UriDrag::decode(e, strList) ) return;

    Q3StrList stringList;

    Q3StrListIterator it(strList);

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

}  // namespace KIPIAdvancedSlideshowPlugin
