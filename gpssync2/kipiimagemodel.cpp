/* ============================================================
 *
 * Date        : 2010-03-21
 * Description : A model to hold information about images
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

#include "kipiimagemodel.moc"

#include <kdebug.h>

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
};

KipiImageModel::KipiImageModel(QObject* const parent)
: QAbstractItemModel(parent), d(new KipiImageModelPrivate)
{
    
}

KipiImageModel::~KipiImageModel()
{
    // TODO: send a signal before deleting the items?
    qDeleteAll(d->items);

    delete d;
}

int KipiImageModel::columnCount(const QModelIndex& parent) const
{
    return d->columnCount;
}

QVariant KipiImageModel::data(const QModelIndex& index, int role) const
{
    const int rowNumber = index.row();
    if ((rowNumber<0)||(rowNumber>=d->items.count()))
    {
        return QVariant();
    }

    return d->items.at(rowNumber)->data(index.column(), role);
}

QModelIndex KipiImageModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        // there are no child items, only top level items
        return QModelIndex();
    }

    if ( (column>=d->columnCount)
         || (row>=d->items.count()) )
        return QModelIndex();

    return createIndex(row, column, 0);
}

QModelIndex KipiImageModel::parent(const QModelIndex& index) const
{
    // we have only top level items
    return QModelIndex();
}

void KipiImageModel::addItem(KipiImageItem* const newItem)
{
    beginInsertRows(QModelIndex(), d->items.count(), d->items.count()+1);
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

    const QModelIndex itemModelIndex = createIndex(itemIndex, 0, 0);
    emit(dataChanged(itemModelIndex, itemModelIndex));
}

KipiImageItem* KipiImageModel::itemFromIndex(const QModelIndex& index) const
{
    const int row = index.row();
    if (row>=d->items.count())
        return 0;

    return d->items.at(row);
}

int KipiImageModel::rowCount(const QModelIndex& parent) const
{
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
    const int rowNumber = index.row();
    if ((rowNumber<0)||(rowNumber>=d->items.count()))
    {
        return false;
    }

    const bool success = d->items.at(rowNumber)->setData(index.column(), role, value);
    if (success)
    {
        // we have to assume that all columns changed:
        emit(dataChanged(index, index.sibling(index.row(), d->columnCount-1)));
    }
    return success;
}

Qt::ItemFlags KipiImageModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
}

} /* KIPIGPSSyncPlugin */
