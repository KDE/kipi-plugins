/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : a widget to display the imagelist
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include <QPixmap>
#include <QTreeWidget>
#include <QWidget>

// KDE includes.

#include <kurl.h>

namespace KIPI
{
class Interface;
}

namespace KIPIRemoveRedEyesPlugin
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

    ImagesListView(QWidget *parent = 0);
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

    explicit ImagesList(KIPI::Interface *iface, QWidget* parent = 0);
    ~ImagesList();

    KUrl::List imageUrls() const;
    int processed()        const;
    int failed()           const;

    bool hasUnprocessedImages();

    void removeItemByUrl(const KUrl& url);
    void removeUnprocessedImages();
    void resetEyeCounterColumn();

signals:

    void imageListChanged(bool);
    void foundRAWImages(bool);

public slots:

    void addImages(const KUrl::List& list);
    void addEyeCounterByUrl(const KUrl&, int);


private slots:

    void addItems();
    void removeItems();
    void thumbnail(const KUrl& url, const QPixmap& pix);
    void updateSummary();

private:

    bool isRAWFile(const QString& filePath);

private:

    ImagesListPriv* const d;
};

}  // namespace KIPIRemoveRedEyesPlugin

#endif // IMAGES_LIST_H
