/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGES_LIST_H
#define IMAGES_LIST_H

// Qt includes.

#include <qlistview.h>
#include <qwidget.h>
#include <qpixmap.h>

// KDE includes.

#include <kurl.h>

class KFileItem;

namespace KIPI
{
    class Interface;
}

namespace KIPIFlickrExportPlugin
{

class ImagesPagePriv;

class ImagesListViewItem : public QListViewItem
{

public:

    ImagesListViewItem(QListView *view, const KURL& url);
    ~ImagesListViewItem();

    void setUrl(const KURL& url);
    KURL url() const;

    void setThumb(const QPixmap& pix);

private: 

    KURL m_url;
};

// ---------------------------------------------------------

class ImagesListView : public QListView
{
    Q_OBJECT

public:

    ImagesListView(QWidget *parent);
    ~ImagesListView();

signals:

    void addedDropedItems(const KURL::List& urls);

private:

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
};

// ---------------------------------------------------------

class ImagesList : public QWidget
{
    Q_OBJECT

public:

    ImagesList(QWidget* parent, KIPI::Interface *iface);
    ~ImagesList();

    KURL::List imageUrls() const;

signals:

    void signalImageListChanged(bool);

public slots:

    void slotAddImages(const KURL::List& list);

private slots:

    void slotAddItems();
    void slotRemoveItems();
    void slotGotThumbnail(const KFileItem *item, const QPixmap& pix);

private:

    ImagesPagePriv *d;
};

}  // namespace KIPIFlickrExportPlugin

#endif // IMAGES_LIST_H
