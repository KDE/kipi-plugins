/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-21
 * Description : Central object for managing bookmarks
 *
 * Copyright (C) 2009 by Michael G. Hansen <mike at mghansen dot de>
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

#include "gpsbookmarkowner.h"
#include "gpsbookmarkowner.moc"

// KDE includes

#include <kactioncollection.h>
#include <kbookmarkmenu.h>
#include <kbookmarkmanager.h>
#include <kstandarddirs.h>

// local includes

#include "inputboxnocancel.h"

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
};

GPSBookmarkOwner::GPSBookmarkOwner(QWidget* const parent)
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
    const QString title = InputBoxNoCancel::AskForString(i18n("Bookmark location"), i18n("Title:"), currentUrl(), d->parent);

    return title;
}

QString GPSBookmarkOwner::currentUrl() const
{
    if (!positionProviderFunction)
        return QString();

    GPSDataContainer position;
    if (!positionProviderFunction(&position, positionProviderFunctionData))
        return QString();

    return position.geoUrl();
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
    const GPSDataContainer position = GPSDataContainer::fromGeoUrl(url, &okay);

    if (okay)
        emit(positionSelected(position));
}

void GPSBookmarkOwner::setPositionProvider(PositionProviderFunction function, void* yourdata)
{
    positionProviderFunction = function;
    positionProviderFunctionData = yourdata;
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

}  // namespace KIPIGPSSyncPlugin
