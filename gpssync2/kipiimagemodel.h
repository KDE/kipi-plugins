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

#ifndef KIPIIMAGEMODEL_H
#define KIPIIMAGEMODEL_H

// Qt includes

#include <QAbstractItemModel>
#include <QPixmap>

// libKIPI includes

#include <libkipi/interface.h>

// local includes

#include "kipiimageitem.h"

namespace KIPIGPSSyncPlugin
{

class KipiImageModelPrivate;

class KipiImageModel : public QAbstractItemModel
{
Q_OBJECT

public:
    KipiImageModel(QObject* const parent = 0);
    ~KipiImageModel();

    // QAbstractItemModel:
    virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    // own functions:
    void addItem(KipiImageItem* const newItem);
    void setColumnCount(const int nColumns);
    KipiImageItem* itemFromIndex(const QModelIndex& index) const;
    KipiImageItem* itemFromUrl(const KUrl& url) const;
    QModelIndex indexFromUrl(const KUrl& url) const;

    QPixmap getPixmapForIndex(const QPersistentModelIndex& itemIndex, const int size);
    void setKipiInterface(KIPI::Interface* const interface);

protected:
    void itemChanged(KipiImageItem* const changedItem);

Q_SIGNALS:
    void signalThumbnailForIndexAvailable(const QPersistentModelIndex& index, const QPixmap& pixmap);

protected Q_SLOTS:
    void slotThumbnailFromInterface(const KUrl& url, const QPixmap& pixmap);

private:
    KipiImageModelPrivate* const d;

    friend class KipiImageItem;
};

} /* KIPIGPSSyncPlugin */

// TODO: ugly way to prevent double declaration of the metatype
#ifndef WORLDMAPWIDGET2_PRIMITIVES_H
Q_DECLARE_METATYPE(QPersistentModelIndex);
#endif /* WORLDMAPWIDGET2_PRIMITIVES_H */

#endif /* KIPIIMAGEMODEL_H */
