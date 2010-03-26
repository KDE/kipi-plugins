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

#include <QTreeView>

// local includes

#include "kipiimagemodel.h"

namespace KIPIGPSSyncPlugin
{

class KipiImageListPrivate;

class KipiImageListDragDropHandler : public QObject
{
Q_OBJECT

public:
    KipiImageListDragDropHandler(QObject* const parent = 0);
    virtual ~KipiImageListDragDropHandler();

    virtual QMimeData* createMimeData(const QList<QPersistentModelIndex>& modelIndices) = 0;
};

class KipiImageListViewInternalPrivate;

class KipiImageListViewInternal : public QTreeView
{
Q_OBJECT

public:
    KipiImageListViewInternal(QWidget* const parent = 0);
    ~KipiImageListViewInternal();

    void setDragDropHandler(KipiImageListDragDropHandler* const dragDropHandler);

protected:
    virtual void startDrag(Qt::DropActions supportedActions);

private:
    KipiImageListViewInternalPrivate* const d;
};

class KipiImageList : public QWidget
{
Q_OBJECT

public:
    KipiImageList(QWidget* const parent = 0);
    ~KipiImageList();

    void setModel(QAbstractItemModel* const model, QItemSelectionModel* const selectionModel);
    QTreeView* view() const;
    void setDragDropHandler(KipiImageListDragDropHandler* const dragDropHandler);

private:
    KipiImageListPrivate* const d;
};

} /* KIPIGPSSyncPlugin */

#endif /* KIPIIMAGELIST_H */
