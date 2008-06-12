/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : images list settings page.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QTreeWidget>
#include <QWidget>
#include <QPixmap>

// KDE includes.

#include <kurl.h>

namespace KIPI
{
    class Interface;
}

namespace KIPIFlickrExportPlugin
{

class ImagesListPriv;

class ImagesListViewItem : public QTreeWidgetItem
{

public:

    ImagesListViewItem(QTreeWidget *view, const KUrl& url);
    ~ImagesListViewItem();

    void setUrl(const KUrl& url);
    KUrl url() const;

    void setThumb(const QPixmap& pix);

private:

    KUrl m_url;
};

// -------------------------------------------------------------------------

class ImagesListView : public QTreeWidget
{

    Q_OBJECT

public:

    ImagesListView(QWidget *parent);
    ~ImagesListView();

signals:

    void addedDropedItems(const KUrl::List& urls);

private:

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
};

// -------------------------------------------------------------------------

class ImagesList : public QWidget
{
    Q_OBJECT

public:

    ImagesList(QWidget* parent, KIPI::Interface *iface);
    ~ImagesList();

    KUrl::List imageUrls() const;
    void removeItemByUrl(const KUrl& url);

signals:

    void signalImageListChanged(bool);

public slots:

    void slotAddImages(const KUrl::List& list);

private slots:


    void slotAddItems();
    void slotRemoveItems();
    void slotThumbnail(const KUrl& url, const QPixmap& pix);

private:

    ImagesListPriv* d;
};

}  // namespace KIPIFlickrExportPlugin

#endif // IMAGES_LIST_H 
