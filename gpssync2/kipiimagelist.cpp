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

#include "kipiimagelist.moc"

// Qt includes

#include <QDrag>
#include <QHeaderView>
#include <QPainter>
#include <QVBoxLayout>
#include <QWheelEvent>

// KDE includes

#include <kconfiggroup.h>
#include <kdebug.h>
#include <kiconloader.h>

// libKIPI includes

#include <libkipi/interface.h>

namespace KIPIGPSSyncPlugin
{

KipiImageListDragDropHandler::KipiImageListDragDropHandler(QObject* const parent)
: QObject(parent)
{
}

KipiImageListDragDropHandler::~KipiImageListDragDropHandler()
{
}

class KipiImageListViewInternalPrivate
{
public:
    KipiImageListViewInternalPrivate()
    : dragDropHandler(0)
    {
    }

    KipiImageListDragDropHandler* dragDropHandler;
    KipiImageList* kipiImageList;
};

KipiImageListViewInternal::KipiImageListViewInternal(KipiImageList* const parent)
: QTreeView(parent), d(new KipiImageListViewInternalPrivate())
{
    d->kipiImageList = parent;
    header()->setMovable(true);
    setUniformRowHeights(true);
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
}

KipiImageListViewInternal::~KipiImageListViewInternal()
{
    delete d;
}

void KipiImageListViewInternal::setDragDropHandler(KipiImageListDragDropHandler* const dragDropHandler)
{
    d->dragDropHandler = dragDropHandler;
}

void KipiImageListViewInternal::startDrag(Qt::DropActions supportedActions)
{
    if (!d->dragDropHandler)
    {
        QTreeView::startDrag(supportedActions);
        return;
    }

    const QList<QModelIndex> selectedIndicesFromModel = selectionModel()->selectedIndexes();
    QList<QPersistentModelIndex> selectedIndices;
    for (int i=0; i<selectedIndicesFromModel.count(); ++i)
    {
        selectedIndices << selectedIndicesFromModel.at(i);
    }
    QMimeData* const dragMimeData = d->dragDropHandler->createMimeData(selectedIndices);

    if (!dragMimeData)
        return;

    QDrag* const drag = new QDrag(this);
    drag->setMimeData(dragMimeData);
    drag->start(Qt::CopyAction);
}

class KipiImageListPrivate
{
public:
    KipiImageListPrivate()
    : model(0),
      selectionModel(0),
      treeView(0),
      itemDelegate(0),
      interface(0)
    {
    }

    KipiImageModel* model;
    QItemSelectionModel* selectionModel;
    KipiImageListViewInternal* treeView;
    KipiImageItemDelegate* itemDelegate;
    KIPI::Interface* interface;
};

KipiImageList::KipiImageList(KIPI::Interface* const interface, QWidget* const parent)
: QWidget(parent), d(new KipiImageListPrivate())
{
    d->interface = interface;

    QVBoxLayout* const vBoxLayout = new QVBoxLayout(this);
    d->treeView = new KipiImageListViewInternal(this);
    vBoxLayout->addWidget(d->treeView);

    setLayout(vBoxLayout);

    d->itemDelegate = new KipiImageItemDelegate(this, this);
    d->treeView->setItemDelegate(d->itemDelegate);
    setThumbnailSize(60);
}

KipiImageList::~KipiImageList()
{
    delete d;
}

void KipiImageList::setModel(KipiImageModel* const model, QItemSelectionModel* const selectionModel)
{
    d->model = model;
    d->selectionModel = selectionModel;
    d->treeView->setModel(model);

    connect(d->model, SIGNAL(signalThumbnailForIndexAvailable(const QPersistentModelIndex&, const QPixmap&)),
            this, SLOT(slotThumbnailFromModel(const QPersistentModelIndex&, const QPixmap&)));

    if (selectionModel)
        d->treeView->setSelectionModel(selectionModel);
}

QTreeView* KipiImageList::view() const
{
    return d->treeView;
}

void KipiImageList::setDragDropHandler(KipiImageListDragDropHandler* const dragDropHandler)
{
    d->treeView->setDragDropHandler(dragDropHandler);
}

class KipiImageItemDelegatePrivate
{
public:
    KipiImageItemDelegatePrivate()
    : imageList(0),
      thumbnailSize(60)
    {
    }

    KipiImageList* imageList;
    int thumbnailSize;
};

KipiImageItemDelegate::KipiImageItemDelegate(KipiImageList* const imageList, QObject* const parent)
: QItemDelegate(parent), d(new KipiImageItemDelegatePrivate())
{
    d->imageList = imageList;
}

KipiImageItemDelegate::~KipiImageItemDelegate()
{
    delete d;
}

void KipiImageItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column()!=KipiImageItem::ColumnThumbnail)
    {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    // TODO: clipping, selected state, disabled state, etc.
    QPixmap itemPixmap = d->imageList->getModel()->getPixmapForIndex(index, d->thumbnailSize);
    if (itemPixmap.isNull())
    {
        // TODO: paint some default logo
        // TODO: cache this logo
        itemPixmap = SmallIcon("image-x-generic", d->thumbnailSize, KIconLoader::DisabledState);
    }

    const QSize availableSize = option.rect.size();
    const QSize pixmapSize = itemPixmap.size().boundedTo(availableSize);
    QPoint startPoint((availableSize.width()-pixmapSize.width())/2,
                      (availableSize.height()-pixmapSize.height())/2);
    startPoint+=option.rect.topLeft();
    painter->drawPixmap(QRect(startPoint, pixmapSize), itemPixmap, QRect(QPoint(0, 0), pixmapSize));
}

QSize KipiImageItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column()==KipiImageItem::ColumnThumbnail)
    {
        return QSize(d->thumbnailSize, d->thumbnailSize);
    }

    return QItemDelegate::sizeHint(option, index);
}

void KipiImageItemDelegate::setThumbnailSize(const int size)
{
    d->thumbnailSize = size;

    KipiImageModel* const imageModel = d->imageList->getModel();
    if (!imageModel)
        return;

    if (imageModel->rowCount()>0)
    {
        // TODO: is it enough to emit this signal for only 1 item?
        // seems to work in Qt4.5 with QTreeView::setUniformRowHeights(true)
        emit(sizeHintChanged(imageModel->index(0, 0)));
    }
}

int KipiImageItemDelegate::getThumbnailSize() const
{
    return d->thumbnailSize;
}

KipiImageModel* KipiImageList::getModel() const
{
    return d->model;
}

void KipiImageList::setThumbnailSize(const int size)
{
    d->itemDelegate->setThumbnailSize(size);
    d->treeView->setColumnWidth(KipiImageItem::ColumnThumbnail, size);
}

void KipiImageList::slotIncreaseThumbnailSize()
{
    // TODO: pick reasonable limits and make sure we stay on multiples of 5
    const int currentThumbnailSize = d->itemDelegate->getThumbnailSize();
    if (currentThumbnailSize<200)
        setThumbnailSize(currentThumbnailSize+5);
}

void KipiImageList::slotDecreaseThumbnailSize()
{
    const int currentThumbnailSize = d->itemDelegate->getThumbnailSize();
    if (currentThumbnailSize>30)
        setThumbnailSize(currentThumbnailSize-5);
}

void KipiImageListViewInternal::wheelEvent(QWheelEvent* we)
{
    if ((we->modifiers()&Qt::ControlModifier) == 0)
    {
        QTreeView::wheelEvent(we);
        return;
    }

    we->accept();
    if (we->delta()>0)
    {
        d->kipiImageList->slotIncreaseThumbnailSize();
    }
    else
    {
        d->kipiImageList->slotDecreaseThumbnailSize();
    }
}

void KipiImageList::slotThumbnailFromModel(const QPersistentModelIndex& index, const QPixmap& pixmap)
{
    // TODO: verify that the size corresponds to the size of our thumbnails!
    d->treeView->update(index);
}

void KipiImageList::saveSettingsToGroup(KConfigGroup* const group)
{
    group->writeEntry("Image List Thumbnail Size", d->itemDelegate->getThumbnailSize());
}

void KipiImageList::readSettingsFromGroup(KConfigGroup* const group)
{
    // TODO: the initial column width is not set properly
    d->itemDelegate->setThumbnailSize(group->readEntry("Image List Thumbnail Size", 60));
}

} /* KIPIGPSSyncPlugin */
