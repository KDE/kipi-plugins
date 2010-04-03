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

// KDE includes

#include <kdebug.h>
#include <kiconloader.h>
#include <kpixmapcache.h>

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
};

KipiImageListViewInternal::KipiImageListViewInternal(QWidget* const parent)
: QTreeView(parent), d(new KipiImageListViewInternalPrivate())
{
    header()->setMovable(true);
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
    {
    }

    KipiImageModel* model;
    QItemSelectionModel* selectionModel;
    KipiImageListViewInternal* treeView;
    KipiImageItemDelegate* itemDelegate;
    KPixmapCache* pixmapCache;
    KIPI::Interface* interface;
};

KipiImageList::KipiImageList(KIPI::Interface* const interface, QWidget* const parent)
: QWidget(parent), d(new KipiImageListPrivate())
{
    d->interface = interface;

    if (d->interface)
    {
        connect(d->interface, SIGNAL(gotThumbnail(const KUrl&, const QPixmap&)),
                this, SLOT(slotThumbnailFromInterface(const KUrl&, const QPixmap&)));
    }

    // TODO: find an appropriate name
    d->pixmapCache = new KPixmapCache("somename");

    QVBoxLayout* const vBoxLayout = new QVBoxLayout(this);
    d->treeView = new KipiImageListViewInternal(this);
    vBoxLayout->addWidget(d->treeView);

    setLayout(vBoxLayout);

    d->itemDelegate = new KipiImageItemDelegate(this, this);
    d->treeView->setItemDelegate(d->itemDelegate);
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
      thumbnailSize(60, 60)
    {
    }

    KipiImageList* imageList;
    QSize thumbnailSize;
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

    // TODO: clipping, selected state, disabled state, etc.
    QPixmap itemPixmap = d->imageList->getPixmapForIndex(index, d->thumbnailSize);
    if (itemPixmap.isNull())
    {
        // TODO: paint some default logo
        // TODO: cache this logo
        itemPixmap = SmallIcon("image-x-generic", qMax(d->thumbnailSize.width(), d->thumbnailSize.height()), KIconLoader::DisabledState);
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
        return d->thumbnailSize;
    }

    return QItemDelegate::sizeHint(option, index);
}

void KipiImageItemDelegate::setThumbnailSize(const QSize& size)
{
    d->thumbnailSize = size;
}

static QString CacheKeyFromSizeAndUrl(const QSize& size, const KUrl& url)
{
    return QString("%1-%3").arg(qMax(size.width(), size.height())).arg(url.pathOrUrl());
}

QPixmap KipiImageList::getPixmapForIndex(const QPersistentModelIndex& itemIndex, const QSize& size)
{
    // TODO: should we cache the pixmap on our own here or does the interface usually cache it for us?
    // TODO: do we need to make sure we do not request the same pixmap twice in a row?
    // construct the key under which we stored the pixmap in the cache:
    KipiImageItem* const imageItem = d->model->itemFromIndex(itemIndex);
    if (!imageItem)
        return QPixmap();

    const QString itemKeyString = CacheKeyFromSizeAndUrl(size, imageItem->url());
    QPixmap thumbnailPixmap;
    const bool havePixmapInCache = d->pixmapCache->find(itemKeyString, thumbnailPixmap);
    kDebug()<<imageItem->url()<<size<<havePixmapInCache<<d->pixmapCache->isEnabled();
    if (havePixmapInCache)
        return thumbnailPixmap;

    // TODO: what about raw images? The old version of the plugin had a special loading mechanism for those
    if (d->interface)
    {
        d->interface->thumbnails(KUrl::List()<<imageItem->url(), qMax(size.width(), size.height()));
    }
    else
    {
//         KIO::PreviewJob *job = KIO::filePreview(urls, DEFAULTSIZE);
// 
//         connect(job, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)),
//                 this, SLOT(slotKDEPreview(const KFileItem&, const QPixmap&)));
// 
//         connect(job, SIGNAL(failed(const KFileItem&)),
//                 this, SLOT(slotKDEPreviewFailed(const KFileItem&)));
    }

    return QPixmap();
}

KipiImageModel* KipiImageList::getModel() const
{
    return d->model;
}

void KipiImageList::slotThumbnailFromInterface(const KUrl& url, const QPixmap& pixmap)
{
    kDebug()<<url<<pixmap.size();
    if (pixmap.isNull())
        return;

    // save the pixmap:
    const QString itemKeyString = CacheKeyFromSizeAndUrl(pixmap.size(), url);
    d->pixmapCache->insert(itemKeyString, pixmap);

    // find the item corresponding to the URL:
    const QModelIndex imageIndex = d->model->indexFromUrl(url);
    if (imageIndex.isValid())
        d->treeView->update(imageIndex);
}

} /* KIPIGPSSyncPlugin */
