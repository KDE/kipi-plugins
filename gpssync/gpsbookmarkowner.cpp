/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2009-11-21
 * @brief  Central object for managing bookmarks
 *
 * @author Copyright (C) 2009,2010 by Michael G. Hansen
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
    KMap::GeoCoordinates lastCoordinates;
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
    const KMap::GeoCoordinates coordinate = KMap::GeoCoordinates::fromGeoUrl(url, &okay);

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
    KUrl bookmarkIconUrl;
    bool visible;

    void addBookmarkGroupToModel(const KBookmarkGroup& group);
    
};

GPSBookmarkModelHelper::GPSBookmarkModelHelper(KBookmarkManager* const bookmarkManager, KipiImageModel* const kipiImageModel, QObject* const parent)
: ModelHelper(parent), d(new GPSBookmarkModelHelperPrivate())
{
    d->model = new QStandardItemModel(this);
    d->bookmarkManager = bookmarkManager;
    d->kipiImageModel = kipiImageModel;
    d->bookmarkIconUrl = KStandardDirs::locate("data", "gpssync/bookmarks-marker.png");
    d->pixmap = QPixmap(d->bookmarkIconUrl.toLocalFile());

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

bool GPSBookmarkModelHelper::itemCoordinates(const QModelIndex& index, KMap::GeoCoordinates* const coordinates) const
{
    const KMap::GeoCoordinates itemCoordinates = index.data(CoordinatesRole).value<KMap::GeoCoordinates>();

    if (coordinates)
    {
        *coordinates = itemCoordinates;
    }

    return itemCoordinates.hasCoordinates();
}

bool GPSBookmarkModelHelper::itemIcon(const QModelIndex& index, QPoint* const offset, QSize* const size, QPixmap* const pixmap, KUrl* const url) const
{
    Q_UNUSED(index)

    if (offset)
    {
        *offset = QPoint(d->pixmap.width()/2, d->pixmap.height()-1);
    }

    if (url)
    {
        *url = d->bookmarkIconUrl;

        if (size)
        {
            *size = d->pixmap.size();
        }
    }
    else
    {
        *pixmap = d->pixmap;
    }

    return true;
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
            const KMap::GeoCoordinates coordinates = KMap::GeoCoordinates::fromGeoUrl(currentBookmark.url().url(), &okay);
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

void GPSBookmarkOwner::setPositionAndTitle(const KMap::GeoCoordinates& coordinates, const QString& title)
{
    d->lastCoordinates = coordinates;
    d->lastTitle = title;
}

KMap::ModelHelper::Flags GPSBookmarkModelHelper::modelFlags() const
{
    return FlagSnaps|(d->visible?FlagVisible:FlagNull);
}

KMap::ModelHelper::Flags GPSBookmarkModelHelper::itemFlags(const QModelIndex& /*index*/) const
{
    return FlagVisible|FlagSnaps;
}

void GPSBookmarkModelHelper::snapItemsTo(const QModelIndex& targetIndex, const QList<QModelIndex>& snappedIndices)
{
    GPSUndoCommand* const undoCommand = new GPSUndoCommand();

    KMap::GeoCoordinates targetCoordinates;
    if (!itemCoordinates(targetIndex, &targetCoordinates))
        return;

    for (int i=0; i<snappedIndices.count(); ++i)
    {
        const QPersistentModelIndex itemIndex = snappedIndices.at(i);
        KipiImageItem* const item = d->kipiImageModel->itemFromIndex(itemIndex);
        
        GPSDataContainer newData;
        newData.setCoordinates(targetCoordinates);
        
        GPSUndoCommand::UndoInfo undoInfo(itemIndex);
        undoInfo.readOldDataFromItem(item);
 
        item->setGPSData(newData);
        undoInfo.readNewDataFromItem(item);

        //undoCommand->addUndoInfo(GPSUndoCommand::UndoInfo(itemIndex, oldData, newData, oldTagList, newTagList));
        undoCommand->addUndoInfo(undoInfo);
    }
    kDebug()<<targetIndex.data(Qt::DisplayRole).toString();
    undoCommand->setText(i18np("1 image snapped to '%2'",
                               "%1 images snapped to '%2'", snappedIndices.count(), targetIndex.data(Qt::DisplayRole).toString()));

    emit(signalUndoCommand(undoCommand));
}

}  // namespace KIPIGPSSyncPlugin
