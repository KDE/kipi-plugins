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

#include "mapdragdrophandler.moc"

// Qt includes

#include <QDropEvent>

// local includes

#include "gpsimageitem.h"

namespace KIPIGPSSyncPlugin
{

MapDragDropHandler::MapDragDropHandler(QAbstractItemModel* const pModel, QObject* const parent)
 : DragDropHandler(parent), model(pModel)
{
}

MapDragDropHandler::~MapDragDropHandler()
{
}

Qt::DropAction MapDragDropHandler::accepts(const QDropEvent* e)
{
    return Qt::CopyAction;
}

bool MapDragDropHandler::dropEvent(const QDropEvent* e, const WMW2::WMWGeoCoordinate& dropCoordinates, QList<QPersistentModelIndex>* const droppedIndices)
{
    const MapDragData* const mimeData = qobject_cast<const MapDragData*>(e->mimeData());
    if (!mimeData)
        return false;

    for (int i=0; i<mimeData->draggedIndices.count(); ++i)
    {
        const QPersistentModelIndex itemIndex = mimeData->draggedIndices.at(i);
        if (!itemIndex.isValid())
            continue;

        model->setData(itemIndex, QVariant::fromValue(dropCoordinates), GPSImageItem::RoleCoordinates);
    }

    // let the WorldMapWidget2 know which markers were dropped:
    if (droppedIndices)
    {
        *droppedIndices = mimeData->draggedIndices;
    }

    return true;
}

QMimeData* MapDragDropHandler::createMimeData(const QList<QPersistentModelIndex>& modelIndices)
{
    Q_UNUSED(modelIndices);

    return 0;
}

} /* KIPIGPSSyncPlugin */

