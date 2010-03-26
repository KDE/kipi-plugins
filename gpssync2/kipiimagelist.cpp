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
#include <QVBoxLayout>

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

    QAbstractItemModel* model;
    QItemSelectionModel* selectionModel;
    KipiImageListViewInternal* treeView;
};

KipiImageList::KipiImageList(QWidget* const parent)
: QWidget(parent), d(new KipiImageListPrivate())
{
    QVBoxLayout* const vBoxLayout = new QVBoxLayout(this);
    d->treeView = new KipiImageListViewInternal(this);
    vBoxLayout->addWidget(d->treeView);

    setLayout(vBoxLayout);
}

KipiImageList::~KipiImageList()
{
    delete d;
}

void KipiImageList::setModel(QAbstractItemModel* const model, QItemSelectionModel* const selectionModel)
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

} /* KIPIGPSSyncPlugin */
