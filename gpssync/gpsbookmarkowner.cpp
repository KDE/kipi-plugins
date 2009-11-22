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

// Qt includes

#include <QInputDialog>

// KDE includes

#include <kactioncollection.h>
#include <kbookmarkmenu.h>
#include <kbookmarkmanager.h>
#include <kstandarddirs.h>

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
    // right now, even if the user clicks cancel, he cannot abort :-(
    // we would have to return an empty URL, but the we would get an error-message
    const QString title = QInputDialog::getText(d->parent, i18n("Bookmark location"), i18n("Title:"), QLineEdit::Normal, currentUrl());

    if (title.isEmpty())
        return currentUrl();
    
    return title;
}

QString GPSBookmarkOwner::currentUrl() const
{
    if (!positionProviderFunction)
        return QString();

    GPSDataContainer position;
    if (!positionProviderFunction(&position, positionProviderFunctionData))
        return QString();

    const double lat = position.latitude();
    const double lon = position.longitude();
    const double altitude = position.altitude();

    return QString::fromLatin1("geo:%1,%2,%3").arg(lat, 0, 'f', 10).arg(lon, 0, 'f', 10).arg(altitude, 0, 'f', 10);
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

    // parse geo:-uri according to (only partially implemented):
    // http://tools.ietf.org/html/draft-ietf-geopriv-geo-uri-04
    // TODO: verify that we follow the spec fully!
    if (!url.startsWith("geo:"))
    {
        // TODO: error
        return;
    }

    const QStringList parts = url.mid(4).split(',');

    GPSDataContainer position;
    if ((parts.size()==3)||(parts.size()==2))
    {
        bool okay = true;
        double ptLongitude = 0.0;
        double ptLatitude  = 0.0;
        double ptAltitude  = 0.0;

        ptLatitude = parts[0].toDouble(&okay);
        if (okay)
            ptLongitude = parts[1].toDouble(&okay);

        if (okay&&(parts.size()==3))
            ptAltitude = parts[2].toDouble(&okay);

        if (!okay)
        {
            // TODO: error
            return;
        }

        position = GPSDataContainer(ptAltitude, ptLatitude, ptLongitude, false);
    }
    else
    {
        // TODO: error
        return;
    }
    
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
