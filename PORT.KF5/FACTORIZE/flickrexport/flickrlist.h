/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

// KDE includes

#include <klineedit.h>

// Local includes

#include "kpimageslist.h"

using namespace KIPIPlugins;

namespace KIPIFlickrExportPlugin
{

class FlickrList : public KPImagesList
{
    Q_OBJECT

public:

    /* The different columns in a Flickr list. */
    enum FieldType
    {
        SAFETYLEVEL = KPImagesListView::User1,
        CONTENTTYPE = KPImagesListView::User2,
        TAGS        = KPImagesListView::User3,
        PUBLIC      = KPImagesListView::User4,
        FAMILY      = KPImagesListView::User5,
        FRIENDS     = KPImagesListView::User6
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

    explicit FlickrList(QWidget* const parent = 0, bool = false);

    void setPublic(Qt::CheckState);
    void setFamily(Qt::CheckState);
    void setFriends(Qt::CheckState);
    void setSafetyLevels(SafetyLevel);
    void setContentTypes(ContentType);

Q_SIGNALS:

    // Signal for notifying when the states of one of the permission columns has
    // changed. The first argument specifies which permission has changed, the
    // second the state.
    void signalPermissionChanged(FlickrList::FieldType, Qt::CheckState);

    void signalSafetyLevelChanged(FlickrList::SafetyLevel);
    void signalContentTypeChanged(FlickrList::ContentType);

public Q_SLOTS:

    virtual void slotAddImages(const KUrl::List& list);

private:

    void setPermissionState(FieldType, Qt::CheckState);
    void singlePermissionChanged(QTreeWidgetItem*, int);
    void singleComboBoxChanged(QTreeWidgetItem*, int);


private Q_SLOTS:

    void slotItemChanged(QTreeWidgetItem*, int);
    void slotItemClicked(QTreeWidgetItem*, int);

private:

    Qt::CheckState          m_public;
    Qt::CheckState          m_family;
    Qt::CheckState          m_friends;
    FlickrList::SafetyLevel m_safetyLevel;
    FlickrList::ContentType m_contentType;

    // Used to separate the ImagesList::itemChanged signals that were caused
    // programmatically from those caused by the user.
    bool                    m_userIsEditing;

    bool                    m_is23;
};

// -------------------------------------------------------------------------

class FlickrListViewItem : public KPImagesListViewItem
{

public:

    FlickrListViewItem(KPImagesListView* const view, const KUrl& url,
                       bool, bool, bool, bool,
                       FlickrList::SafetyLevel, FlickrList::ContentType);
    //virtual ~FlickrListViewItem();

    void setPublic(bool);
    void setFamily(bool);
    void setFriends(bool);
    void setSafetyLevel(FlickrList::SafetyLevel);
    void setContentType(FlickrList::ContentType);
    bool isPublic() const;
    bool isFamily() const;
    bool isFriends() const;
    FlickrList::SafetyLevel safetyLevel() const;
    FlickrList::ContentType contentType() const;

    /**
     * Returns the list of extra tags that the user specified for this image.
     */
    QStringList extraTags() const;

    /* This method should be called when one of the checkboxes is clicked. */
    void toggled();

    virtual void updateItemWidgets();

private:

    bool                    m_is23;

    bool                    m_public;
    bool                    m_family;
    bool                    m_friends;

    FlickrList::SafetyLevel m_safetyLevel;
    FlickrList::ContentType m_contentType;

    /**
     * LineEdit used for extra tags per image.
     */
    KLineEdit*              m_tagLineEdit;
};

} // namespace KIPIFlickrExportPlugin

#endif /* FLICKRLIST_H */
