/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : widget to display an imagelist
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGESLIST_H
#define IMAGESLIST_H

// Qt includes.

#include <QStringList>
#include <QTreeWidget>
#include <QWidget>

// KDE includes.

#include <kurl.h>

// LibKIPI includes.

#include <libkipi/imageinfo.h>


#include "kipiplugins_export.h"

namespace KIPI
{
class Interface;
}

namespace KIPIPlugins
{

class ImagesList;
class ImagesListView;
class ImagesListPriv;

class KIPIPLUGINS_EXPORT ImagesListViewItem : public QTreeWidgetItem
{

public:

    ImagesListViewItem(ImagesListView *view, const KUrl& url);
    ~ImagesListViewItem();

    void setUrl(const KUrl& url);
    KUrl url() const;

    void setComments(const QString& comments);
    QString comments();

    void setTags(const QStringList& tags);
    QStringList tags();

    void setRating(int rating);
    int rating();

    void setThumb(const QPixmap& pix);

private:

    int         m_rating;         // Image Rating from Kipi host.

    QString     m_comments;       // Image comments from Kipi host.

    QStringList m_tags;           // List of keywords from Kipi host.

    KUrl        m_url;            // Image url provided by Kipi host.
};

// -------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT ImagesListView : public QTreeWidget
{
    Q_OBJECT

public:

    enum ColumnType
    {
        Thumbnail = 0,
        Filename,
        User1,
        User2,
        User3,
        User4,
        User5,
        User6
    };

    ImagesListView(ImagesList *parent = 0);
    ~ImagesListView();

    void setColumnLabel(ColumnType column, const QString &label);
    void setColumnEnabled(ColumnType column, bool enable);
    void setColumn(ColumnType column, const QString &label, bool enable);

    KIPI::Interface* iface() const;

signals:

    void addedDropedItems(const KUrl::List& urls);

private:

    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
};

// -------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT ImagesList : public QWidget
{
    Q_OBJECT

public:

    enum ControlButtonPlacement
    {
        NoControlButtons = 0,
        ControlButtonsRight,
        ControlButtonsBelow
    };

    explicit ImagesList(KIPI::Interface *iface, QWidget* parent = 0, 
                        ControlButtonPlacement btnPlace = ControlButtonsRight);
    virtual ~ImagesList();

    void setAllowRAW(bool allow);
    void loadImagesFromCurrentSelection();

    ImagesListView*  listView() const;
    KIPI::Interface* iface() const;

    virtual KUrl::List imageUrls() const;
    virtual void removeItemByUrl(const KUrl& url);

signals:

    void signalImageListChanged(bool);
    void signalFoundRAWImages(bool);

public slots:

    virtual void slotAddImages(const KUrl::List& list);

protected:

    QWidget* plainPage() const;

protected slots:

    virtual void slotAddItems();
    virtual void slotRemoveItems();
    virtual void slotThumbnail(const KUrl& url, const QPixmap& pix);

private:

    bool isRAWFile(const QString& filePath);

private:

    ImagesListPriv* const d;
};

}  // namespace KIPIPlugins

#endif // IMAGESLIST_H
