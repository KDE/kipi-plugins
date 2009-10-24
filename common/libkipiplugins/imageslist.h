/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-05-21
 * Description : widget to display an imagelist
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

// Qt includes

#include <QPushButton>
#include <QStringList>
#include <QTreeWidget>
#include <QWidget>

// KDE includes

#include <kurl.h>
#include <kicon.h>
#include <kiconloader.h>

// LibKIPI includes

#include <libkipi/imageinfo.h>

// Local includes

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

    ImagesListViewItem(ImagesListView* view, const KUrl& url);
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

    void updateInformation();

private:

    int             m_rating;         // Image Rating from Kipi host.
    QString         m_comments;       // Image comments from Kipi host.
    QStringList     m_tags;           // List of keywords from Kipi host.
    KUrl            m_url;            // Image url provided by Kipi host.
    ImagesListView* m_view;
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
    ImagesListView(int iconSize, ImagesList *parent = 0);
    ~ImagesListView();

    void setColumnLabel(ColumnType column, const QString& label);
    void setColumnEnabled(ColumnType column, bool enable);
    void setColumn(ColumnType column, const QString& label, bool enable);

    KIPI::Interface* iface() const;

Q_SIGNALS:

    void addedDropedItems(const KUrl::List& urls);
    void signalItemClicked(QTreeWidgetItem*);

private Q_SLOTS:

    void slotItemClicked(QTreeWidgetItem* item, int column);

public:
    void enableDragAndDrop(const bool enable = true);

private:

    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);

    void setup(int iconSize);

private:

    int m_iconSize;
};

// -------------------------------------------------------------------------

class CtrlButton : public QPushButton
{
    Q_OBJECT

public:

    CtrlButton(const QIcon& icon, QWidget* parent = 0);
    virtual ~CtrlButton();
};

// -------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT ImagesList : public QWidget
{
    Q_OBJECT

public:

    enum ControlButtonPlacement
    {
        NoControlButtons = 0,
        ControlButtonsLeft,
        ControlButtonsRight,
        ControlButtonsAbove,
        ControlButtonsBelow
    };

    enum ControlButton
    {
       Add       = 0x1,
       Remove    = 0x2,
       MoveUp    = 0x4,
       MoveDown  = 0x8,
       Clear     = 0x10,
       Load      = 0x20,
       Save      = 0x40
    };
    Q_DECLARE_FLAGS(ControlButtons, ControlButton)

    ImagesList(KIPI::Interface* iface, QWidget* parent = 0, int iconSize = -1);
    virtual ~ImagesList();

    void               setAllowRAW(bool allow);
    void               loadImagesFromCurrentSelection();

    int                iconSize()  const;

    ImagesListView*    listView()  const;
    KIPI::Interface*   iface()     const;

    virtual KUrl::List imageUrls() const;
    virtual void       removeItemByUrl(const KUrl& url);

    void               setControlButtons(ControlButtons buttonMask);
    void               setControlButtonsPlacement(ControlButtonPlacement placement);
    void               enableControlButtons(bool enable = true);
    void               enableDragAndDrop(const bool enable = true);

Q_SIGNALS:

    void signalImageListChanged();
    void signalFoundRAWImages(bool);
    void signalItemClicked(QTreeWidgetItem*);

public Q_SLOTS:

    virtual void slotAddImages(const KUrl::List& list);


protected Q_SLOTS:

    virtual void slotAddItems();
    virtual void slotRemoveItems();
    virtual void slotMoveUpItems();
    virtual void slotMoveDownItems();
    virtual void slotClearItems();
    virtual void slotLoadItems() {};
    virtual void slotSaveItems() {};

    virtual void slotThumbnail(const KUrl& url, const QPixmap& pix);
    virtual void slotImageListChanged();

private:

    bool isRAWFile(const QString& filePath);
    void setIconSize(int size);

private:

    ImagesListPriv* const d;
};

}  // namespace KIPIPlugins

Q_DECLARE_OPERATORS_FOR_FLAGS(KIPIPlugins::ImagesList::ControlButtons)

#endif // IMAGESLIST_H
