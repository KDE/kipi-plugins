/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-05-22
 * Description : Flickr/23HQ file list view and items.
 *
 * Copyright (C) 2009 by Pieter Edelman <pieter dot edelman at gmx dot net>
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

#ifndef FLICKRLIST_H
#define FLICKRLIST_H

// Local includes

#include "imageslist.h"

namespace KIPIFlickrExportPlugin
{

class FlickrList : public KIPIPlugins::ImagesList
{
    Q_OBJECT

public:

    /* The different columns in a Flickr list. */
    enum FieldType
    {
        SAFETYLEVEL = KIPIPlugins::ImagesListView::User1,
        CONTENTTYPE = KIPIPlugins::ImagesListView::User2,
        PUBLIC      = KIPIPlugins::ImagesListView::User3,
        FAMILY      = KIPIPlugins::ImagesListView::User4,
        FRIENDS     = KIPIPlugins::ImagesListView::User5
    };

    /* The different possible safety levels recognized by Flickr. */
    enum SafetyLevel
    {
        SAFE        = 1,
        MODERATE    = 2,
        RESTRICTED  = 3,
        MIXEDLEVELS = -1
    };

    /* The different possible content types recognized by Flickr. */
    enum ContentType
    {
        PHOTO      = 1,
        SCREENSHOT = 2,
        OTHER      = 3,
        MIXEDTYPES = -1
    };

public:
    FlickrList(KIPI::Interface *iface,
               QWidget* parent = 0,
               bool = false,
               ControlButtonPlacement btnPlace = ControlButtonsRight);

    void setPublic(Qt::CheckState);
    void setFamily(Qt::CheckState);
    void setFriends(Qt::CheckState);

signals:
    // Signal for notifying when the states of one of the permission columns has
    // changed. The first argument specifies which permission has changed, the
    // second the state.
    void signalPermissionChanged(FlickrList::FieldType, Qt::CheckState);

public slots:
    virtual void slotAddImages(const KUrl::List& list);

private:
    void setPermissionState(FieldType, Qt::CheckState);

    Qt::CheckState m_public;
    Qt::CheckState m_family;
    Qt::CheckState m_friends;
    bool m_is23;

private slots:
    void slotSinglePermissionChanged(QTreeWidgetItem *, int);
};

class FlickrListViewItem : public KIPIPlugins::ImagesListViewItem
{

public:

    FlickrListViewItem(KIPIPlugins::ImagesListView *view, const KUrl& url,
                       bool, bool, bool, bool);
//    ~FlickListViewItem();

    void setPublic(bool);
    void setFamily(bool);
    void setFriends(bool);
    bool isPublic();
    bool isFamily();
    bool isFriends();

    void toggled();

private:
    bool m_is23;

    bool m_public;
    bool m_family;
    bool m_friends;
};

} // namespace KIPIFlickrExportPlugin

#endif /* FLICKRLIST_H */
