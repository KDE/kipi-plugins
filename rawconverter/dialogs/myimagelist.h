/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : file list view and items.
 *
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

namespace KIPIRawConverterPlugin
{

class MyImageList : public KPImagesList
{
    Q_OBJECT

public:

    /* The different columns in a list. */
    enum FieldType
    {
        TARGETFILENAME = KPImagesListView::User1,
        IDENTIFICATION = KPImagesListView::User2,
        STATUS         = KPImagesListView::User3
    };

public:

    explicit MyImageList(QWidget* const parent = 0);
    virtual ~MyImageList();

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

    void setDestFileName(const QString& str);
    QString destFileName() const;

    void setIdentity(const QString& str);
    QString identity() const;

    void setStatus(const QString& str);

    QString destPath() const;

private:

    QString m_destFileName;
    QString m_identity;
    QString m_status;
};

} // namespace KIPIRawConverterPlugin

#endif /* MYIMAGELIST_H */
