/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2008-05-21
 * Description : widget to display an imagelist
 *
 * Copyright (C) 2006-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2010 by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2009-2010 by Luka Renko <lure at kubuntu dot org>
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

#ifndef KPIMAGESLIST_H
#define KPIMAGESLIST_H

// Qt includes

#include <QPushButton>
#include <QStringList>
#include <QTreeWidget>
#include <QWidget>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QIcon>
#include <QUrl>

// Local includes

#include "kipiplugins_export.h"

namespace KIPI
{
class Interface;
}

using namespace KIPI;

namespace KIPIPlugins
{

class KPImagesList;
class KPImagesListView;

class KIPIPLUGINS_EXPORT KPImagesListViewItem : public QTreeWidgetItem
{

public:

    enum State
    {
        Waiting,
        Success,
        Failed
    };

public:

    explicit KPImagesListViewItem(KPImagesListView* const view, const QUrl& url);
    ~KPImagesListViewItem();

    bool hasValidThumbnail() const;

    void setUrl(const QUrl& url);
    QUrl url() const;

    void setComments(const QString& comments);
    QString comments() const;

    void setTags(const QStringList& tags);
    QStringList tags() const;

    void setRating(int rating);
    int rating() const;

    void setThumb(const QPixmap& pix, bool hasThumb=true);
    void setProgressAnimation(const QPixmap& pix);

    void setProcessedIcon(const QIcon& icon);
    void setState(State state);
    State state() const;

    void updateInformation();

    // implement this, if you have special item widgets, e.g. an edit line
    // they will be set automatically when adding items, changing order, etc.
    virtual void updateItemWidgets() {};

protected:

    KPImagesListView* view() const;

private:

    void setPixmap(const QPixmap& pix);

private:

    class Private;
    Private* const d;
};

// -------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPImagesListView : public QTreeWidget
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

    explicit KPImagesListView(KPImagesList* const parent = nullptr);
    explicit KPImagesListView(int iconSize, KPImagesList* const parent = nullptr);
    ~KPImagesListView();

    void setColumnLabel(ColumnType column, const QString& label);
    void setColumnEnabled(ColumnType column, bool enable);
    void setColumn(ColumnType column, const QString& label, bool enable);

    KPImagesListViewItem* findItem(const QUrl& url);
    QModelIndex indexFromItem(KPImagesListViewItem* item, int column = 0) const;
    KPImagesListViewItem* getCurrentItem() const;

    Interface* iface() const;

Q_SIGNALS:

    void signalAddedDropedItems(const QList<QUrl>&);
    void signalItemClicked(QTreeWidgetItem*);
    void signalContextMenuRequested();

private Q_SLOTS:

    void slotItemClicked(QTreeWidgetItem* item, int column);

public:

    void enableDragAndDrop(const bool enable = true);

private:

    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dropEvent(QDropEvent* e) override;
    void contextMenuEvent(QContextMenuEvent * e) override;

    void setup(int iconSize);

    void drawRow(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

private:

    int m_iconSize;
};

// -------------------------------------------------------------------------

class CtrlButton : public QPushButton
{
    Q_OBJECT

public:

    explicit CtrlButton(const QIcon& icon, QWidget* parent = nullptr);
    virtual ~CtrlButton();
};

// -------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPImagesList : public QWidget
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

public:

    explicit KPImagesList(QWidget* const parent = nullptr, int iconSize = -1);
    virtual ~KPImagesList();

    void                setAllowRAW(bool allow);
    void                setAllowDuplicate(bool allow);

    void                loadImagesFromCurrentSelection();

    /** A function to load all the images from the album if no image has been selected by user.
     */
    void                loadImagesFromCurrentAlbum();

    int                 iconSize()  const;

    KPImagesListView*   listView()  const;
    Interface*          iface()     const;

    void                processing(const QUrl& url);
    void                processed(const QUrl& url, bool success);
    void                cancelProcess();
    void                clearProcessedStatus();

    void                setControlButtons(ControlButtons buttonMask);
    void                setControlButtonsPlacement(ControlButtonPlacement placement);
    void                enableControlButtons(bool enable = true);
    void                enableDragAndDrop(const bool enable = true);

    void                updateThumbnail(const QUrl& url);

    virtual QList<QUrl> imageUrls(bool onlyUnprocessed = false) const;
    virtual void        removeItemByUrl(const QUrl& url);
    QUrl                getCurrentUrl() const;

Q_SIGNALS:

    void signalAddItems(const QList<QUrl>&);
    void signalMoveUpItem();
    void signalMoveDownItem();
    void signalRemovedItems(const QList<QUrl>&);
    void signalRemovingItem(KIPIPlugins::KPImagesListViewItem*);
    void signalImageListChanged();
    void signalFoundRAWImages(bool);
    void signalItemClicked(QTreeWidgetItem*);
    void signalContextMenuRequested();
    void signalXMLSaveItem(QXmlStreamWriter&, KIPIPlugins::KPImagesListViewItem*);
    void signalXMLLoadImageElement(QXmlStreamReader&);
    void signalXMLCustomElements(QXmlStreamWriter&);
    void signalXMLCustomElements(QXmlStreamReader&);

public Q_SLOTS:

    virtual void slotAddImages(const QList<QUrl>& list);
    virtual void slotRemoveItems();

protected Q_SLOTS:

    void slotProgressTimerDone();

    virtual void slotAddItems();
    virtual void slotMoveUpItems();
    virtual void slotMoveDownItems();
    virtual void slotClearItems();
    virtual void slotLoadItems();
    virtual void slotSaveItems();
    virtual void slotThumbnail(const QUrl& url, const QPixmap& pix);
    virtual void slotImageListChanged();

private:

    void setIconSize(int size);
    bool isRawFile(const QUrl& url) const;

private:

    class Private;
    Private* const d;
};

}  // namespace KIPIPlugins

Q_DECLARE_OPERATORS_FOR_FLAGS(KIPIPlugins::KPImagesList::ControlButtons)

#endif // KPIMAGESLIST_H
