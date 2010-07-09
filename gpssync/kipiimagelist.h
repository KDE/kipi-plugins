/* ============================================================
 *
 * Date        : 2010-03-22
 * Description : A view to display a list of images
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef KIPIIMAGELIST_H
#define KIPIIMAGELIST_H

// Qt includes

#include <QItemDelegate>
#include <QTreeView>

// libKIPI includes

#include <libkipi/interface.h>

// local includes

#include "kipiimagemodel.h"

class QWheelEvent;
class KConfigGroup;

namespace KIPIGPSSyncPlugin
{

class KipiImageListPrivate;
class KipiImageSortProxyModel;

class KipiImageListDragDropHandler : public QObject
{
Q_OBJECT

public:
    KipiImageListDragDropHandler(QObject* const parent = 0);
    virtual ~KipiImageListDragDropHandler();

    virtual QMimeData* createMimeData(const QList<QPersistentModelIndex>& modelIndices) = 0;
};

class KipiImageListViewInternalPrivate;
class KipiImageList;

class KipiImageListViewInternal : public QTreeView
{
Q_OBJECT

public:
    KipiImageListViewInternal(KipiImageList* const parent = 0);
    ~KipiImageListViewInternal();

    void setDragDropHandler(KipiImageListDragDropHandler* const dragDropHandler);

protected:
    virtual void startDrag(Qt::DropActions supportedActions);
    virtual void wheelEvent(QWheelEvent* we);

private:
    KipiImageListViewInternalPrivate* const d;
};

class KipiImageList : public QWidget
{
Q_OBJECT

public:
    KipiImageList(KIPI::Interface* const interface, QWidget* const parent = 0);
    ~KipiImageList();

    void setModel(KipiImageModel* const model, QItemSelectionModel* const selectionModel);
    QTreeView* view() const;
    KipiImageModel* getModel() const;
    QItemSelectionModel* getSelectionModel() const;
    void setDragDropHandler(KipiImageListDragDropHandler* const dragDropHandler);
    void setThumbnailSize(const int size);
    KipiImageSortProxyModel* getSortProxyModel() const;

    void saveSettingsToGroup(KConfigGroup* const group);
    void readSettingsFromGroup(const KConfigGroup* const group);
    void setEditEnabled(const bool state);
    void setDragEnabled(const bool state);

Q_SIGNALS:
    void signalImageActivated(const QModelIndex& index);

public Q_SLOTS:
    void slotIncreaseThumbnailSize();
    void slotDecreaseThumbnailSize();
    void slotUpdateActionsEnabled();

private Q_SLOTS:
    void slotThumbnailFromModel(const QPersistentModelIndex& index, const QPixmap& pixmap);
    void slotInternalTreeViewImageActivated(const QModelIndex& index);
    void slotColumnVisibilityActionTriggered(QAction* action);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    KipiImageListPrivate* const d;
};

class KipiImageItemDelegatePrivate;

class KipiImageItemDelegate : public QItemDelegate
{
Q_OBJECT
public:
    KipiImageItemDelegate(KipiImageList* const imageList, QObject* const parent = 0);
    virtual ~KipiImageItemDelegate();

    void setThumbnailSize(const int size);
    int getThumbnailSize() const;
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& sortMappedindex) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& sortMappedindex) const;

private:
    KipiImageItemDelegatePrivate* const d;
    
};

} /* KIPIGPSSyncPlugin */

#endif /* KIPIIMAGELIST_H */
