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

#ifndef GPSBOOKMARKOWNER_H
#define GPSBOOKMARKOWNER_H

// KDE includes:

#include <kbookmarkmanager.h>

// local includes:

#include "gpsdatacontainer.h"

class KBookmarkManager;

namespace KIPIGPSSyncPlugin
{

class GPSBookmarkModelHelperPrivate;
class GPSBookmarkModelHelper : public WMW2::WMWModelHelper
{
Q_OBJECT
public:
    enum Constants
    {
        CoordinatesRole = Qt::UserRole + 1
    };

    GPSBookmarkModelHelper(KBookmarkManager* const bookmarkManager, QObject* const parent = 0);
    virtual ~GPSBookmarkModelHelper();

    virtual QAbstractItemModel* model() const;
    virtual QItemSelectionModel* selectionModel() const;
    virtual bool itemCoordinates(const QModelIndex& index, WMW2::WMWGeoCoordinate* const coordinates) const;
    virtual QPixmap itemIcon(const QModelIndex& index, QPoint* const offset) const;

private Q_SLOTS:
    void slotUpdateBookmarksModel();

private:
    GPSBookmarkModelHelperPrivate* const d;
};

class GPSBookmarkOwnerPrivate;
class GPSBookmarkOwner : public QObject, public KBookmarkOwner
{
    Q_OBJECT

public:
    GPSBookmarkOwner(QWidget* const parent);
    virtual ~GPSBookmarkOwner();

    virtual bool supportsTabs() const;
    virtual QString currentTitle() const;
    virtual QString currentUrl() const;
    virtual bool enableOption(BookmarkOption option) const;
    virtual void openBookmark(const KBookmark&, Qt::MouseButtons, Qt::KeyboardModifiers);

    KMenu* getMenu() const;

    typedef bool (*PositionProviderFunction)(GPSDataContainer* position, void* yourdata);
    PositionProviderFunction positionProviderFunction;
    void* positionProviderFunctionData;
    void setPositionProvider(const PositionProviderFunction function, void* const yourdata);
    void changeAddBookmark(const bool state);

    KBookmarkManager* bookmarkManager() const;

    GPSBookmarkModelHelper* bookmarkModelHelper() const;

Q_SIGNALS:
    void positionSelected(GPSDataContainer position);

private:
    GPSBookmarkOwnerPrivate* const d;

};

}  // namespace KIPIGPSSyncPlugin

#endif // GPSBOOKMARKOWNER_H


