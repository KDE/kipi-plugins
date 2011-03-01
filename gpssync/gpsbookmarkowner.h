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

#ifndef GPSBOOKMARKOWNER_H
#define GPSBOOKMARKOWNER_H

// KDE includes:

#include <kbookmarkmanager.h>

// libkmap includes

#include <libkmap/modelhelper.h>

// local includes:

#include "gpsdatacontainer.h"

class KBookmarkManager;

namespace KIPIGPSSyncPlugin
{

class KipiImageModel;
class GPSUndoCommand;
class GPSBookmarkModelHelperPrivate;
class GPSBookmarkModelHelper : public KMap::ModelHelper
{
Q_OBJECT
public:
    enum Constants
    {
        CoordinatesRole = Qt::UserRole + 1
    };

    GPSBookmarkModelHelper(KBookmarkManager* const bookmarkManager, KipiImageModel* const kipiImageModel, QObject* const parent = 0);
    virtual ~GPSBookmarkModelHelper();

    virtual QAbstractItemModel* model() const;
    virtual QItemSelectionModel* selectionModel() const;
    virtual bool itemCoordinates(const QModelIndex& index, KMap::GeoCoordinates* const coordinates) const;
    virtual bool itemIcon(const QModelIndex& index, QPoint* const offset, QSize* const size, QPixmap* const pixmap, KUrl* const url) const;
    virtual Flags modelFlags() const;
    virtual Flags itemFlags(const QModelIndex& index) const;
    virtual void snapItemsTo(const QModelIndex& targetIndex, const QList<QModelIndex>& snappedIndices);

    void setVisible(const bool state);

private Q_SLOTS:
    void slotUpdateBookmarksModel();


Q_SIGNALS:
    void signalUndoCommand(GPSUndoCommand* undoCommand);

private:
    GPSBookmarkModelHelperPrivate* const d;
};

class GPSBookmarkOwnerPrivate;
class GPSBookmarkOwner : public QObject, public KBookmarkOwner
{
    Q_OBJECT

public:
    GPSBookmarkOwner(KipiImageModel* const kipiImageModel, QWidget* const parent);
    virtual ~GPSBookmarkOwner();

    virtual bool supportsTabs() const;
    virtual QString currentTitle() const;
    virtual QString currentUrl() const;
    virtual bool enableOption(BookmarkOption option) const;
    virtual void openBookmark(const KBookmark&, Qt::MouseButtons, Qt::KeyboardModifiers);

    KMenu* getMenu() const;

    void changeAddBookmark(const bool state);
    void setPositionAndTitle(const KMap::GeoCoordinates& coordinates, const QString& title);

    KBookmarkManager* bookmarkManager() const;

    GPSBookmarkModelHelper* bookmarkModelHelper() const;

Q_SIGNALS:
    void positionSelected(GPSDataContainer position);

private:
    GPSBookmarkOwnerPrivate* const d;

};

}  // namespace KIPIGPSSyncPlugin

#endif // GPSBOOKMARKOWNER_H


