/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-03-22
 * @brief  Drag-and-drop handler for KMap integration.
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

#ifndef MAPDRAGDROPHANDLER_H
#define MAPDRAGDROPHANDLER_H

// Qt includes

#include <QAbstractItemModel>
#include <QMimeData>

// Libkmap includes

#include <libkmap/dragdrophandler.h>

namespace KIPIGPSSyncPlugin
{

class GPSSyncKMapModelHelper;

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

class MapDragDropHandler : public KMap::DragDropHandler
{
Q_OBJECT

public:
    MapDragDropHandler(QAbstractItemModel* const pModel, GPSSyncKMapModelHelper* const parent);
    virtual ~MapDragDropHandler();

    virtual Qt::DropAction accepts(const QDropEvent* e);
    virtual bool dropEvent(const QDropEvent* e, const KMap::GeoCoordinates& dropCoordinates);
    virtual QMimeData* createMimeData(const QList<QPersistentModelIndex>& modelIndices);

private:
    QAbstractItemModel* const model;
    GPSSyncKMapModelHelper* const gpsSyncKMapModelHelper;
};

} /* KIPIGPSSyncPlugin */

#endif /* MAPDRAGDROPHANDLER_H */
