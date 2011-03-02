/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-03-21
 * @brief  A model to hold information about images.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

#include "kipiimagemodel.moc"

// KDE includes

#include <kdebug.h>
#include <klinkitemselectionmodel.h>
#include <kpixmapcache.h>

namespace KIPIGPSSyncPlugin
{

class KipiImageModelPrivate
{
public:
    KipiImageModelPrivate()
    : items(),
      columnCount(0)
    {
    }

    QList<KipiImageItem*> items;
    int columnCount;
    QMap<QPair<int, int>, QVariant> headerData;
    KPixmapCache* pixmapCache;
    KIPI::Interface* interface;
    QList<QPair<QPersistentModelIndex, int> > requestedPixmaps;
};

KipiImageModel::KipiImageModel(QObject* const parent)
: QAbstractItemModel(parent), d(new KipiImageModelPrivate)
{
    // TODO: find an appropriate name
    d->pixmapCache = new KPixmapCache("somename");
}

KipiImageModel::~KipiImageModel()
{
    // TODO: send a signal before deleting the items?
    qDeleteAll(d->items);
    delete d->pixmapCache;
    delete d;
}

int KipiImageModel::columnCount(const QModelIndex& /*parent*/) const
{
    return d->columnCount;
}

QVariant KipiImageModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid()) { Q_ASSERT(index.model()==this); }
    const int rowNumber = index.row();
    if ((rowNumber<0)||(rowNumber>=d->items.count()))
    {
        return QVariant();
    }

    return d->items.at(rowNumber)->data(index.column(), role);
}

QModelIndex KipiImageModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid()) { Q_ASSERT(parent.model()==this); }
//     kDebug()<<row<<column<<parent;
    if (parent.isValid())
    {
        // there are no child items, only top level items
        return QModelIndex();
    }

    if ( (column<0) || (column>=d->columnCount)
         || (row<0) || (row>=d->items.count()) )
        return QModelIndex();

    return createIndex(row, column, 0);
}

QModelIndex KipiImageModel::parent(const QModelIndex& /*index*/) const
{
    // we have only top level items
    return QModelIndex();
}

void KipiImageModel::addItem(KipiImageItem* const newItem)
{
    beginInsertRows(QModelIndex(), d->items.count(), d->items.count());
    newItem->setModel(this);
    d->items << newItem;
    endInsertRows();
}

void KipiImageModel::setColumnCount(const int nColumns)
{
    emit(layoutAboutToBeChanged());
    d->columnCount = nColumns;
    emit(layoutChanged());
}

void KipiImageModel::itemChanged(KipiImageItem* const changedItem)
{
    const int itemIndex = d->items.indexOf(changedItem);
    if (itemIndex<0)
        return;

    const QModelIndex itemModelIndexStart = createIndex(itemIndex, 0, 0);
    const QModelIndex itemModelIndexEnd = createIndex(itemIndex, d->columnCount - 1, 0);
    emit(dataChanged(itemModelIndexStart, itemModelIndexEnd));
}

KipiImageItem* KipiImageModel::itemFromIndex(const QModelIndex& index) const
{
    if (index.isValid()) { Q_ASSERT(index.model()==this); }
    if (!index.isValid())
        return 0;

    const int row = index.row();
    if ((row<0)||(row>=d->items.count()))
        return 0;

    return d->items.at(row);
}

int KipiImageModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) { Q_ASSERT(parent.model()==this); }
    if (parent.isValid())
        return 0;

    return d->items.count();
}

bool KipiImageModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if ((section>=d->columnCount)||(orientation!=Qt::Horizontal))
        return false;

    const QPair<int, int> headerIndex = QPair<int, int>(section, role);
    d->headerData[headerIndex] = value;

    return true;
}

QVariant KipiImageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((section>=d->columnCount)||(orientation!=Qt::Horizontal))
        return false;

    const QPair<int, int> headerIndex = QPair<int, int>(section, role);
    return d->headerData.value(headerIndex);
}

bool KipiImageModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;
}

Qt::ItemFlags KipiImageModel::flags(const QModelIndex& index) const
{
    if (index.isValid()) { Q_ASSERT(index.model()==this); }

    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
}

KipiImageItem* KipiImageModel::itemFromUrl(const KUrl& url) const
{
    for (int i=0; i<d->items.count(); ++i)
    {
        if (d->items.at(i)->url()==url)
            return d->items.at(i);
    }

    return 0;
}

QModelIndex KipiImageModel::indexFromUrl(const KUrl& url) const
{
    for (int i=0; i<d->items.count(); ++i)
    {
        if (d->items.at(i)->url()==url)
            return index(i, 0, QModelIndex());
    }

    return QModelIndex();
}

static QString CacheKeyFromSizeAndUrl(const int size, const KUrl& url)
{
    return QString("%1-%3").arg(size).arg(url.pathOrUrl());
}

QPixmap KipiImageModel::getPixmapForIndex(const QPersistentModelIndex& itemIndex, const int size)
{
    if (itemIndex.isValid()) { Q_ASSERT(itemIndex.model()==this); }

    // TODO: should we cache the pixmap on our own here or does the interface usually cache it for us?
    // TODO: do we need to make sure we do not request the same pixmap twice in a row?
    // construct the key under which we stored the pixmap in the cache:
    KipiImageItem* const imageItem = itemFromIndex(itemIndex);
    if (!imageItem)
        return QPixmap();

    const QString itemKeyString = CacheKeyFromSizeAndUrl(size, imageItem->url());
    QPixmap thumbnailPixmap;
    const bool havePixmapInCache = d->pixmapCache->find(itemKeyString, thumbnailPixmap);
//     kDebug()<<imageItem->url()<<size<<havePixmapInCache<<d->pixmapCache->isEnabled();
    if (havePixmapInCache)
        return thumbnailPixmap;

    // did we already request this pixmap at this size?
    for (int i=0; i<d->requestedPixmaps.count(); ++i)
    {
        if (d->requestedPixmaps.at(i).first==itemIndex)
        {
            if (d->requestedPixmaps.at(i).second==size)
            {
                // the pixmap has already been requested, at this size
                return QPixmap();
            }
        }
    }

    // remember at which size the pixmap was ordered:
    d->requestedPixmaps << QPair<QPersistentModelIndex, int>(itemIndex, size);

    // TODO: what about raw images? The old version of the plugin had a special loading mechanism for those
    if (d->interface)
    {
        d->interface->thumbnails(KUrl::List()<<imageItem->url(), size);
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

void KipiImageModel::slotThumbnailFromInterface(const KUrl& url, const QPixmap& pixmap)
{
    kDebug()<<url<<pixmap.size();
    if (pixmap.isNull())
        return;

    const int effectiveSize = qMax(pixmap.size().width(), pixmap.size().height());

    // find the item corresponding to the URL:
    const QModelIndex imageIndex = indexFromUrl(url);
    kDebug()<<url<<imageIndex.isValid();
    if (imageIndex.isValid())
    {
        // this is tricky: some kipi interfaces return pixmaps at the requested size, others do not.
        // therefore we check whether a pixmap of this size has been requested. If so, we send it on.
        // If a pixmap of this size has not been requested, we rescale it to fulfill all other requests.

        // index, size
        QList<QPair<int, int> > openRequests;
        for (int i=0; i<d->requestedPixmaps.count(); ++i)
        {
            if (d->requestedPixmaps.at(i).first==imageIndex)
            {
                const int requestedSize = d->requestedPixmaps.at(i).second;
                if (requestedSize==effectiveSize)
                {
                    // match, send it out.
                    d->requestedPixmaps.removeAt(i);
                    kDebug()<<i;

                    // save the pixmap:
                    const QString itemKeyString = CacheKeyFromSizeAndUrl(effectiveSize, url);
                    d->pixmapCache->insert(itemKeyString, pixmap);

                    emit(signalThumbnailForIndexAvailable(imageIndex, pixmap));
                    return;
                }
                else
                {
                    openRequests << QPair<int, int>(i, requestedSize);
                }
            }
        }

        // the pixmap was not requested at this size, fulfill all requests:
        for (int i=openRequests.count()-1; i>=0; --i)
        {
            const int targetSize = openRequests.at(i).second;
            d->requestedPixmaps.removeAt(openRequests.at(i).first);
            kDebug()<<i<<targetSize;

            QPixmap scaledPixmap = pixmap.scaled(targetSize, targetSize, Qt::KeepAspectRatio);

            // save the pixmap:
            const QString itemKeyString = CacheKeyFromSizeAndUrl(targetSize, url);
            d->pixmapCache->insert(itemKeyString, scaledPixmap);

            emit(signalThumbnailForIndexAvailable(imageIndex, scaledPixmap));
        }
        
    }
}

void KipiImageModel::setKipiInterface(KIPI::Interface* const interface)
{
    d->interface = interface;

    connect(d->interface, SIGNAL(gotThumbnail(const KUrl&, const QPixmap&)),
            this, SLOT(slotThumbnailFromInterface(const KUrl&, const QPixmap&)));
}

class KipiImageSortProxyModelPrivate
{
public:
    KipiImageSortProxyModelPrivate()
    {
    }

    KipiImageModel* imageModel;
    QItemSelectionModel* sourceSelectionModel;
    KLinkItemSelectionModel* linkItemSelectionModel;
};

KipiImageSortProxyModel::KipiImageSortProxyModel(KipiImageModel* const kipiImageModel, QItemSelectionModel* const sourceSelectionModel)
: QSortFilterProxyModel(kipiImageModel), d(new KipiImageSortProxyModelPrivate())
{
    d->imageModel = kipiImageModel;
    d->sourceSelectionModel = sourceSelectionModel;
    setSourceModel(kipiImageModel);
    d->linkItemSelectionModel = new KLinkItemSelectionModel(this, d->sourceSelectionModel);
}

KipiImageSortProxyModel::~KipiImageSortProxyModel()
{
    delete d;
}

bool KipiImageSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if ((!left.isValid())||(!right.isValid()))
    {
//         kDebug()<<"INVALID INDICES"<<left<<right;
        return false;
    }

    const int column = left.column();

    const KipiImageItem* const itemLeft = d->imageModel->itemFromIndex(left);
    const KipiImageItem* const itemRight = d->imageModel->itemFromIndex(right);

//     kDebug()<<itemLeft<<itemRight<<column<<rowCount()<<d->imageModel->rowCount();
    return itemLeft->lessThan(itemRight, column);
}

QItemSelectionModel* KipiImageSortProxyModel::mappedSelectionModel()
{
    return d->linkItemSelectionModel;
}

} /* KIPIGPSSyncPlugin */
