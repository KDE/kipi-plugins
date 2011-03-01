/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-03-22
 * @brief  A view to display a list of images.
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

#include "kipiimagelist.moc"

// Qt includes

#include <QDrag>
#include <QHeaderView>
#include <QPainter>
#include <QVBoxLayout>
#include <QWheelEvent>

// KDE includes

#include <kaction.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmenu.h>

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

class KipiImageListPrivate
{
public:
    KipiImageListPrivate()
    : editEnabled(true),
      dragEnabled(false),
      model(0),
      selectionModel(0),
      itemDelegate(0),
      interface(0),
      imageSortProxyModel(0),
      dragDropHandler(0)
    {
    }

    bool editEnabled;
    bool dragEnabled;
    KipiImageModel* model;
    QItemSelectionModel* selectionModel;
    KipiImageItemDelegate* itemDelegate;
    KIPI::Interface* interface;
    KipiImageSortProxyModel *imageSortProxyModel;
    KipiImageListDragDropHandler* dragDropHandler;
};

void KipiImageList::startDrag(Qt::DropActions supportedActions)
{
    if (!d->dragDropHandler)
    {
        QTreeView::startDrag(supportedActions);
        return;
    }

    // NOTE: read the selected indices from the source selection model, not our selection model,
    // which is for the sorted model!
    const QList<QModelIndex> selectedIndicesFromModel = d->selectionModel->selectedIndexes();
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

KipiImageList::KipiImageList(KIPI::Interface* const interface, QWidget* const parent)
: QTreeView(parent), d(new KipiImageListPrivate())
{
    d->interface = interface;

    header()->setMovable(true);
    setUniformRowHeights(true);
    setRootIsDecorated(false);
    setAlternatingRowColors(true);

    d->itemDelegate = new KipiImageItemDelegate(this, this);
    setItemDelegate(d->itemDelegate);
    setThumbnailSize(60);
    slotUpdateActionsEnabled();

    header()->installEventFilter(this);
}

KipiImageList::~KipiImageList()
{
    delete d;
}

void KipiImageList::setModelAndSelectionModel(KipiImageModel* const model, QItemSelectionModel* const selectionModel)
{
    d->model = model;
    d->selectionModel = selectionModel;
    d->imageSortProxyModel = new KipiImageSortProxyModel(d->model, d->selectionModel);
    setModel(d->imageSortProxyModel);

    connect(d->model, SIGNAL(signalThumbnailForIndexAvailable(const QPersistentModelIndex&, const QPixmap&)),
            this, SLOT(slotThumbnailFromModel(const QPersistentModelIndex&, const QPixmap&)));

    connect(this, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(slotInternalTreeViewImageActivated(const QModelIndex&)));

    if (d->imageSortProxyModel->mappedSelectionModel())
        setSelectionModel(d->imageSortProxyModel->mappedSelectionModel());
}

void KipiImageList::setDragDropHandler(KipiImageListDragDropHandler* const dragDropHandler)
{
    d->dragDropHandler = dragDropHandler;
}

class KipiImageItemDelegatePrivate
{
public:
    KipiImageItemDelegatePrivate()
    : imageList(0),
      thumbnailSize(60)
    {
    }

    KipiImageList* imageList;
    int thumbnailSize;
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

void KipiImageItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& sortMappedindex) const
{
    if (sortMappedindex.column()!=KipiImageItem::ColumnThumbnail)
    {
        QItemDelegate::paint(painter, option, sortMappedindex);
        return;
    }

    const QModelIndex& sourceModelIndex = d->imageList->getSortProxyModel()->mapToSource(sortMappedindex);

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    // TODO: clipping, selected state, disabled state, etc.
    QPixmap itemPixmap = d->imageList->getModel()->getPixmapForIndex(sourceModelIndex, d->thumbnailSize);
    if (itemPixmap.isNull())
    {
        // TODO: paint some default logo
        // TODO: cache this logo
        itemPixmap = SmallIcon("image-x-generic", d->thumbnailSize, KIconLoader::DisabledState);
    }

    const QSize availableSize = option.rect.size();
    const QSize pixmapSize = itemPixmap.size().boundedTo(availableSize);
    QPoint startPoint((availableSize.width()-pixmapSize.width())/2,
                      (availableSize.height()-pixmapSize.height())/2);
    startPoint+=option.rect.topLeft();
    painter->drawPixmap(QRect(startPoint, pixmapSize), itemPixmap, QRect(QPoint(0, 0), pixmapSize));
}

QSize KipiImageItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& sortMappedindex) const
{
    if (sortMappedindex.column()==KipiImageItem::ColumnThumbnail)
    {
        return QSize(d->thumbnailSize, d->thumbnailSize);
    }

    const QSize realSizeHint = QItemDelegate::sizeHint(option, sortMappedindex);

    return QSize(realSizeHint.width(), d->thumbnailSize);
}

void KipiImageItemDelegate::setThumbnailSize(const int size)
{
    d->thumbnailSize = size;

    KipiImageModel* const imageModel = d->imageList->getModel();
    if (!imageModel)
        return;

    if (imageModel->rowCount()>0)
    {
        // TODO: is it enough to emit this signal for only 1 item?
        // seems to work in Qt4.5 with QTreeView::setUniformRowHeights(true)
        emit(sizeHintChanged(imageModel->index(0, 0)));
    }
}

int KipiImageItemDelegate::getThumbnailSize() const
{
    return d->thumbnailSize;
}

KipiImageModel* KipiImageList::getModel() const
{
    return d->model;
}

void KipiImageList::setThumbnailSize(const int size)
{
    d->itemDelegate->setThumbnailSize(size);
    setColumnWidth(KipiImageItem::ColumnThumbnail, size);
}

void KipiImageList::slotIncreaseThumbnailSize()
{
    // TODO: pick reasonable limits and make sure we stay on multiples of 5
    const int currentThumbnailSize = d->itemDelegate->getThumbnailSize();
    if (currentThumbnailSize<200)
        setThumbnailSize(currentThumbnailSize+5);
}

void KipiImageList::slotDecreaseThumbnailSize()
{
    const int currentThumbnailSize = d->itemDelegate->getThumbnailSize();
    if (currentThumbnailSize>30)
        setThumbnailSize(currentThumbnailSize-5);
}

void KipiImageList::wheelEvent(QWheelEvent* we)
{
    if ((we->modifiers()&Qt::ControlModifier) == 0)
    {
        QTreeView::wheelEvent(we);
        return;
    }

    we->accept();
    if (we->delta()>0)
    {
        slotIncreaseThumbnailSize();
    }
    else
    {
        slotDecreaseThumbnailSize();
    }
}

void KipiImageList::slotThumbnailFromModel(const QPersistentModelIndex& index, const QPixmap& /*pixmap*/)
{
    // TODO: verify that the size corresponds to the size of our thumbnails!
    update(d->imageSortProxyModel->mapFromSource(index));
}

void KipiImageList::saveSettingsToGroup(KConfigGroup* const group)
{
    group->writeEntry("Image List Thumbnail Size", d->itemDelegate->getThumbnailSize());
    group->writeEntry("Header State", header()->saveState());
}

void KipiImageList::readSettingsFromGroup(const KConfigGroup* const group)
{
    setThumbnailSize(group->readEntry("Image List Thumbnail Size", 60));

    const QByteArray headerState = group->readEntry("Header State", QByteArray());
    if (!headerState.isEmpty())
    {
        header()->restoreState(headerState);
    }
    else
    {
        // by default, hide the advanced columns:
        header()->setSectionHidden(KipiImageItem::ColumnDOP, true);
        header()->setSectionHidden(KipiImageItem::ColumnFixType, true);
        header()->setSectionHidden(KipiImageItem::ColumnNSatellites, true);
    }
}

QItemSelectionModel* KipiImageList::getSelectionModel() const
{
    return d->selectionModel;
}

void KipiImageList::slotInternalTreeViewImageActivated(const QModelIndex& index)
{
    kDebug()<<index<<d->imageSortProxyModel->mapToSource(index);
    emit(signalImageActivated(d->imageSortProxyModel->mapToSource(index)));
}

KipiImageSortProxyModel* KipiImageList::getSortProxyModel() const
{
    return d->imageSortProxyModel;
}

void KipiImageList::setEditEnabled(const bool state)
{
    d->editEnabled = state;
    slotUpdateActionsEnabled();
}

void KipiImageList::setDragEnabled(const bool state)
{
    d->dragEnabled = state;
    slotUpdateActionsEnabled();
}

void KipiImageList::slotUpdateActionsEnabled()
{
    QTreeView::setDragEnabled(d->dragEnabled&&d->editEnabled);
    if (d->dragEnabled&&d->editEnabled)
    {
        QTreeView::setDragDropMode(QAbstractItemView::DragOnly);
    }
}

bool KipiImageList::eventFilter(QObject *watched, QEvent *event)
{
    QHeaderView* const headerView = header();
    if ( (watched!=headerView) || (event->type()!=QEvent::ContextMenu) || (!d->model) )
        return QWidget::eventFilter(watched, event);

    QMenu* const menu = new KMenu(this);

    // add action for all the columns
    for (int i=0; i<d->model->columnCount(); ++i)
    {
        const QString columnName = d->model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        const bool isVisible = !headerView->isSectionHidden(i);

        KAction* const columnAction = new KAction(columnName, menu);
        columnAction->setCheckable(true);
        columnAction->setChecked(isVisible);
        columnAction->setData(i);

        menu->addAction(columnAction);
    }

    connect(menu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotColumnVisibilityActionTriggered(QAction*)));

    QContextMenuEvent * const e = static_cast<QContextMenuEvent*>(event);
    menu->exec(e->globalPos());

    return true;
}

void KipiImageList::slotColumnVisibilityActionTriggered(QAction* action)
{
    const int columnNumber = action->data().toInt();
    const bool columnIsVisible = action->isChecked();

    header()->setSectionHidden(columnNumber, !columnIsVisible);
}

} /* KIPIGPSSyncPlugin */
