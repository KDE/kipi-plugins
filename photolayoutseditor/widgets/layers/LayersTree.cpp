/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "LayersTree.moc"
#include "AbstractPhoto.h"
#include "LayersTreeDelegate.h"
#include "LayersTreeMenu.h"

// Qt
#include <QStandardItemModel>
#include <QDebug>
#include <QHeaderView>
#include <QIcon>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QContextMenuEvent>

using namespace KIPIPhotoLayoutsEditor;

LayersTree::LayersTree(QWidget * parent) :
    QTreeView(parent),
    m_menu(new LayersTreeMenu(this))
{
    this->header()->setShown(true);
    this->header()->setMovable(false);
    this->header()->setClickable(false);
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
    this->setDragDropMode(QAbstractItemView::DragDrop);
    this->setDefaultDropAction(Qt::MoveAction);
    this->header()->setResizeMode(QHeaderView::ResizeToContents);
    this->setContextMenuPolicy(Qt::DefaultContextMenu);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setAnimated(true);
    this->setMultiSelection();
    this->setIconSize(QSize(48,48));
}

void LayersTree::setModel(QAbstractItemModel * model)
{
    QTreeView::setModel(model);

    if (!model)
        return;

    QAbstractItemDelegate * delegate1 = this->itemDelegateForColumn(1);
    QAbstractItemDelegate * delegate2 = this->itemDelegateForColumn(2);
    LayersTreeDelegate * layersDelegate1 = dynamic_cast<LayersTreeDelegate*>(delegate1);
    if (!layersDelegate1)
    {
        layersDelegate1 = new LayersTreeDelegate(this);
        this->setItemDelegateForColumn(1,layersDelegate1);
        connect(this, SIGNAL(clicked(QModelIndex)), layersDelegate1, SLOT(itemClicked(QModelIndex)));
        connect(layersDelegate1, SIGNAL(itemRepaintNeeded(QModelIndex)), this, SLOT(update(QModelIndex)));
    }
    LayersTreeDelegate * layersDelegate2 = dynamic_cast<LayersTreeDelegate*>(delegate2);
    if (!layersDelegate2)
        this->setItemDelegateForColumn(2,layersDelegate1);

    if (this->header()->visualIndex(0) != 2)
        this->header()->moveSection(0,2);

    for (int i = model->columnCount()-1; i >= 0; --i)
        this->resizeColumnToContents(i);

    this->hideColumn(0); /// TODO: Remove when tree representation needed instead of list
}

void LayersTree::setSingleSelection()
{
    if (this->selectedIndexes().count() > 1)
        this->setSelection(this->rect(), QItemSelectionModel::Clear);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
}

void LayersTree::setMultiSelection()
{
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void LayersTree::contextMenuEvent(QContextMenuEvent * event)
{
    QModelIndexList indexList = this->selectedIndexes();
    if (indexList.count())
    {
        m_menu->setDeleteEnabled(true);
        m_menu->setMoveDownEnabled(false);
        m_menu->setMoveUpEnabled(false);

        // Disables unsupported move operations
        QModelIndexList::iterator it = indexList.begin();
        QModelIndex startIndex = *it;
        int minRow;
        int maxRow;
        unsigned int sumRows;
        if (!startIndex.isValid())
            goto end_moving_menus;      // It's not so bad as many people think ;) Here 'goto' simplyfies code!
        minRow = it->row();
        maxRow = it->row();
        sumRows = it->row();
        for (++it; it != indexList.end(); ++it)
        {
            if (!it->isValid())
            {
                event->setAccepted(false);
                return;
            }
            else if (startIndex.parent() != it->parent())
                goto end_moving_menus;  // It's not so bad as many people think ;) Here 'goto' simplyfies code!
            if (it->row() < minRow)
                minRow = it->row();
            if (it->row() > maxRow)
                maxRow = it->row();
            sumRows += it->row();
        }

        if ((((minRow+maxRow)*(maxRow-minRow+1))/2.0) == sumRows)
        {
            if (minRow > 0)
                m_menu->setMoveUpEnabled(true);
            if (maxRow+1 < this->model()->rowCount(indexList.first().parent()))
                m_menu->setMoveDownEnabled(true);
        }

        end_moving_menus:

        // Shows menu
        m_menu->exec(event->globalPos());
    }
}

void LayersTree::removeSelectedRows()
{
    emit selectedRowsAboutToBeRemoved();
}

void LayersTree::moveSelectedRowsUp()
{
    emit selectedRowsAboutToBeMovedUp();
}

void LayersTree::moveSelectedRowsDown()
{
    emit selectedRowsAboutToBeMovedDown();
}
