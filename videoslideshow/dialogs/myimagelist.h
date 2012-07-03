/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-06-24
 * Description : file list view and items.
 *
 * Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#ifndef MYIMAGELIST_H
#define MYIMAGELIST_H

// KDE includes

#include <kurl.h>

// Local includes

#include "kpimageslist.h"

using namespace KIPIPlugins;

namespace KIPIVideoSlideShowPlugin
{

class MyImageList : public KPImagesList
{
    Q_OBJECT

public:

    /* The different columns in a list. */
    enum FieldType
    {
        SPECIALEFFECT  = KPImagesListView::User1,
        TIME           = KPImagesListView::User2,
        TRANSITION     = KPImagesListView::User3,
        TRANSSPEED     = KPImagesListView::User4
    };

public:

    explicit MyImageList(QWidget* const parent = 0);
    virtual ~MyImageList();
    int getTotalFrames();

public Q_SLOTS:

    virtual void slotAddImages(const KUrl::List& list);

protected Q_SLOTS:

    virtual void slotRemoveItems();
};

// -------------------------------------------------------------------------

class MyImageListViewItem : public KPImagesListViewItem
{

public:

    MyImageListViewItem(KPImagesListView* const view, const KUrl& url);
    virtual ~MyImageListViewItem();

    void setEffectName(const QString& str);
    QString EffectName() const;

    void setTime(const int time);
    int getTime() const;

    void setTransition(const QString& str);
    QString getTransition() const;

    void setTransitionSpeed(const QString& str);
    QString getTransitionSpeed() const;

private:

    QString m_effect;
    int     m_time;
    QString m_transition;
    QString m_transSpeed;
    QString m_status;
};

} // namespace KIPIVideoSlideShowPlugin

#endif /* MYIMAGELIST_H */
