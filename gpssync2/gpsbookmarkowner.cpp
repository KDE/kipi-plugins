/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-21
 * Description : Central object for managing bookmarks
 *
 * Copyright (C) 2009,2010 by Michael G. Hansen <mike at mghansen dot de>
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

#include "gpsbookmarkowner.moc"

// Qt includes

#include <QStandardItemModel>

// KDE includes

#include <kactioncollection.h>
#include <kbookmarkmenu.h>
#include <kbookmarkmanager.h>
#include <kstandarddirs.h>

// local includes

#include "inputboxnocancel.h"
#include "gpsundocommand.h"
#include "kipiimagemodel.h"

namespace KIPIGPSSyncPlugin
{

class GPSBookmarkOwnerPrivate
{
public:
    GPSBookmarkOwnerPrivate()
    : parent(0),
      actionCollection(0),
      bookmarkManager(0),
      bookmarkMenuController(0),
      bookmarkMenu(0),
      addBookmarkEnabled(true)
    {
    }

    QWidget             *parent;
    KActionCollection   *actionCollection;
    KBookmarkManager    *bookmarkManager;
    KBookmarkMenu       *bookmarkMenuController;
    KMenu               *bookmarkMenu;
    bool                addBookmarkEnabled;
    GPSBookmarkModelHelper *bookmarkModelHelper;
    WMW2::WMWGeoCoordinate lastCoordinates;
    QString             lastTitle;
};

GPSBookmarkOwner::GPSBookmarkOwner(KipiImageModel* const kipiImageModel, QWidget* const parent)
: d(new GPSBookmarkOwnerPrivate())
{
    d->parent = parent;
    
    // TODO: where do we save the bookmarks? right now, they are kipi-specific
    const QString bookmarksFileName = KStandardDirs::locateLocal("data", "kipi/geobookmarks.xml");
    d->actionCollection = new KActionCollection(this);
    d->bookmarkManager = KBookmarkManager::managerForFile(bookmarksFileName, "kipigeobookmarks");
    d->bookmarkManager->setUpdate(true);
    d->bookmarkMenu = new KMenu(parent);
    d->bookmarkMenuController = new KBookmarkMenu(d->bookmarkManager, this, d->bookmarkMenu, d->actionCollection);

    d->bookmarkModelHelper = new GPSBookmarkModelHelper(d->bookmarkManager, kipiImageModel, this);
}

GPSBookmarkOwner::~GPSBookmarkOwner()
{
    delete d;
}

KMenu* GPSBookmarkOwner::getMenu() const
{
    return d->bookmarkMenu;
}

bool GPSBookmarkOwner::supportsTabs() const
{
    return false;
}

QString GPSBookmarkOwner::currentTitle() const
{
    // TODO: how do we tell the bookmark manager to abort if the user aborts?
    // use a dialog that the user can't cancel, this way at least we don't end up
    // with an empty title
    const QString title = InputBoxNoCancel::AskForString(
                    i18n("Bookmark location"),
                    i18nc("Title of the new gps location bookmark", "Title:"),
                    d->lastTitle.isEmpty() ? currentUrl() : d->lastTitle,
                    d->parent);

    return title;
}

QString GPSBookmarkOwner::currentUrl() const
{
    return d->lastCoordinates.geoUrl();
}

bool GPSBookmarkOwner::enableOption(BookmarkOption option) const
{
    switch (option)
    {
        case ShowAddBookmark:
            return d->addBookmarkEnabled;

        case ShowEditBookmark:
            return true;

        default:
            return false;
    }
}

void GPSBookmarkOwner::openBookmark(const KBookmark& bookmark, Qt::MouseButtons, Qt::KeyboardModifiers)
{
    const QString url = bookmark.url().url().toLower();

    bool okay;
    const WMW2::WMWGeoCoordinate coordinate = WMW2::WMWGeoCoordinate::fromGeoUrl(url, &okay);

    if (okay)
    {
        GPSDataContainer position;
        position.setCoordinates(coordinate);
        emit(positionSelected(position));
    }
}

void GPSBookmarkOwner::changeAddBookmark(const bool state)
{
    d->addBookmarkEnabled = state;

    // re-create the menus:
    // TODO: is there an easier way?
    delete d->bookmarkMenuController;
    d->bookmarkMenu->clear();
    d->bookmarkMenuController = new KBookmarkMenu(d->bookmarkManager, this, d->bookmarkMenu, d->actionCollection);
}

KBookmarkManager* GPSBookmarkOwner::bookmarkManager() const
{
    return d->bookmarkManager;
}

class GPSBookmarkModelHelperPrivate
{
public:
    GPSBookmarkModelHelperPrivate()
    : visible(false)
    {
    }

    QStandardItemModel* model;
    KBookmarkManager* bookmarkManager;
    KipiImageModel* kipiImageModel;
    QPixmap pixmap;
    bool visible;

    void addBookmarkGroupToModel(const KBookmarkGroup& group);
    
};

GPSBookmarkModelHelper::GPSBookmarkModelHelper(KBookmarkManager* const bookmarkManager, KipiImageModel* const kipiImageModel, QObject* const parent)
: WMWModelHelper(parent), d(new GPSBookmarkModelHelperPrivate())
{
    d->model = new QStandardItemModel(this);
    d->bookmarkManager = bookmarkManager;
    d->kipiImageModel = kipiImageModel;
    const KUrl markerUrl = KStandardDirs::locate("data", "gpssync2/bookmarks-marker.png");
    d->pixmap = QPixmap(markerUrl.toLocalFile());

    connect(d->bookmarkManager, SIGNAL(bookmarksChanged(QString)),
            this, SLOT(slotUpdateBookmarksModel()));

    connect(d->bookmarkManager, SIGNAL(changed(const QString&, const QString&)),
            this, SLOT(slotUpdateBookmarksModel()));

    slotUpdateBookmarksModel();
}

GPSBookmarkModelHelper::~GPSBookmarkModelHelper()
{
    delete d;
}

QAbstractItemModel* GPSBookmarkModelHelper::model() const
{
    return d->model;
}

QItemSelectionModel* GPSBookmarkModelHelper::selectionModel() const
{
    return 0;
}

bool GPSBookmarkModelHelper::itemCoordinates(const QModelIndex& index, WMW2::WMWGeoCoordinate* const coordinates) const
{
    const WMW2::WMWGeoCoordinate itemCoordinates = index.data(CoordinatesRole).value<WMW2::WMWGeoCoordinate>();

    if (coordinates)
    {
        *coordinates = itemCoordinates;
    }

    return itemCoordinates.hasCoordinates();
}

QPixmap GPSBookmarkModelHelper::itemIcon(const QModelIndex& index, QPoint* const offset) const
{
    if (offset)
    {
        *offset = QPoint(d->pixmap.width()/2, 0);
    }

    return d->pixmap;
}

void GPSBookmarkModelHelperPrivate::addBookmarkGroupToModel(const KBookmarkGroup& group)
{
    KBookmark currentBookmark = group.first();
    while (!currentBookmark.isNull())
    {
        if (currentBookmark.isGroup())
        {
            addBookmarkGroupToModel(currentBookmark.toGroup());
        }
        else
        {
            bool okay = false;
            const WMW2::WMWGeoCoordinate coordinates = WMW2::WMWGeoCoordinate::fromGeoUrl(currentBookmark.url().url(), &okay);
            if (okay)
            {
                QStandardItem* const item = new QStandardItem();
                item->setData(currentBookmark.text(), Qt::DisplayRole);
                item->setData(QVariant::fromValue(coordinates), GPSBookmarkModelHelper::CoordinatesRole);

                model->appendRow(item);
            }
        }

        currentBookmark = group.next(currentBookmark);
    }
}

void GPSBookmarkModelHelper::slotUpdateBookmarksModel()
{
    d->model->clear();

    // iterate trough all bookmarks
    d->addBookmarkGroupToModel(d->bookmarkManager->root());
}

GPSBookmarkModelHelper* GPSBookmarkOwner::bookmarkModelHelper() const
{
    return d->bookmarkModelHelper;
}

void GPSBookmarkModelHelper::setVisible(const bool state)
{
    d->visible = state;
    emit(signalVisibilityChanged());
}

void GPSBookmarkOwner::setPositionAndTitle(const WMW2::WMWGeoCoordinate& coordinates, const QString& title)
{
    d->lastCoordinates = coordinates;
    d->lastTitle = title;
}

WMW2::WMWModelHelper::Flags GPSBookmarkModelHelper::modelFlags() const
{
    return FlagSnaps|(d->visible?FlagVisible:FlagNull);
}

WMW2::WMWModelHelper::Flags GPSBookmarkModelHelper::itemFlags(const QModelIndex& index) const
{
    return FlagVisible|FlagSnaps;
}

void GPSBookmarkModelHelper::snapItemsTo(const QModelIndex& targetIndex, const QList<QModelIndex>& snappedIndices)
{
    GPSUndoCommand* const undoCommand = new GPSUndoCommand();

    WMW2::WMWGeoCoordinate targetCoordinates;
    if (!itemCoordinates(targetIndex, &targetCoordinates))
        return;

    for (int i=0; i<snappedIndices.count(); ++i)
    {
        const QPersistentModelIndex itemIndex = snappedIndices.at(i);
        GPSImageItem* const item = static_cast<GPSImageItem*>(d->kipiImageModel->itemFromIndex(itemIndex));
        const GPSDataContainer oldData = item->gpsData();
        GPSDataContainer newData = oldData;
        newData.setCoordinates(targetCoordinates);
        item->setGPSData(newData);

        undoCommand->addUndoInfo(GPSUndoCommand::UndoInfo(itemIndex, oldData, newData));
    }
    kDebug()<<targetIndex.data(Qt::DisplayRole).toString();
    undoCommand->setText(i18np("1 image snapped to '%2'",
                               "%1 images snapped to '%2'", snappedIndices.count(), targetIndex.data(Qt::DisplayRole).toString()));

    emit(signalUndoCommand(undoCommand));
}

}  // namespace KIPIGPSSyncPlugin
