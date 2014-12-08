/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-08-09
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KIPI_PIIMGLIST_H
#define KIPI_PIIMGLIST_H

// Qt includes

#include <QWidget>

// Local includes

#include "kpimageslist.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPhotivoIntegrationPlugin
{

// ----------------------------------------------------------------------------

class PIImgList : public KPImagesList
{
    Q_OBJECT

public:
    /* The different columns in a list. */
    enum FieldType
    {
        Title           = KPImagesListView::Filename,
        Description     = KPImagesListView::User1
    };

    explicit PIImgList(QWidget* const parent = 0);
    ~PIImgList();

//     // implement this, if you have special item widgets, e.g. an edit line
//     // they will be set automatically when adding items, changing order, etc.
//     virtual void updateItemWidgets();
};

// -------------------------------------------------------------------------

class PIImgListViewItem : public KPImagesListViewItem
{
public:

    PIImgListViewItem(KPImagesListView* const view, const KUrl& url);
    virtual ~PIImgListViewItem();

    void setTitle(const QString& str);
    QString Title() const;

    void setDescription(const QString& str);
    QString Description() const;

private:

    QString m_Title;
    QString m_Description;
};

// ----------------------------------------------------------------------------

} // namespace KIPIPhotivoIntegrationPlugin

#endif // KIPI_PIIMGLIST_H
