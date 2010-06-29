/* ============================================================
 *
 * Date        : 2010-03-22
 * Description : Drag-and-drop handler for WorldMapWidget2
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

#ifndef MAPDRAGDROPHANDLER_H
#define MAPDRAGDROPHANDLER_H

// Qt includes

#include <QAbstractItemModel>

// Libkmap includes

#include <libkmap/kmap_dragdrophandler.h>

namespace KIPIGPSSyncPlugin
{

class MapDragData : public QMimeData
{
Q_OBJECT

public:
    MapDragData()
    : QMimeData(),
      draggedIndices()
    {
    }

    QList<QPersistentModelIndex> draggedIndices;
};

class MapDragDropHandler : public KMapIface::DragDropHandler
{
Q_OBJECT

public:
    MapDragDropHandler(QAbstractItemModel* const pModel, QObject* const parent = 0);
    virtual ~MapDragDropHandler();

    virtual Qt::DropAction accepts(const QDropEvent* e);
    virtual bool dropEvent(const QDropEvent* e, const KMapIface::WMWGeoCoordinate& dropCoordinates, QList<QPersistentModelIndex>* const droppedIndices);
    virtual QMimeData* createMimeData(const QList<QPersistentModelIndex>& modelIndices);

private:
    QAbstractItemModel* const model;
};

} /* KIPIGPSSyncPlugin */

#endif /* MAPDRAGDROPHANDLER_H */
