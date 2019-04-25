/* ============================================================
 *
 * This file is a part of KDE project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#ifndef IMGURIMAGESLIST_H
#define IMGURIMAGESLIST_H

// Qt includes

#include <QWidget>

// Local includes

#include "kpimageslist.h"
#include "imgurapi3.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIImgurPlugin
{

class ImgurImageListViewItem;

class ImgurImagesList : public KPImagesList
{
    Q_OBJECT

public:
    /* The different columns in a list. */
    enum FieldType
    {
        Title           = KPImagesListView::User1,
        Description     = KPImagesListView::User2,
        URL             = KPImagesListView::User3,
        DeleteURL       = KPImagesListView::User4
    };

    ImgurImagesList(QWidget* const parent = nullptr);
    ~ImgurImagesList() override {}

    QList<const ImgurImageListViewItem*> getPendingItems();

public Q_SLOTS:
    void slotAddImages(const QList<QUrl>& list) override;
    void slotSuccess(const ImgurAPI3Result& result);
    void slotDoubleClick(QTreeWidgetItem* element, int i);
};

// -------------------------------------------------------------------------

class ImgurImageListViewItem : public KPImagesListViewItem
{
public:
    ImgurImageListViewItem(KPImagesListView* const view, const QUrl& url);
    ~ImgurImageListViewItem() override {}

    void setTitle(const QString& str);
    QString Title() const;

    void setDescription(const QString& str);
    QString Description() const;

    void setImgurUrl(const QString& str);
    QString ImgurUrl() const;

    void setImgurDeleteUrl(const QString& str);
    QString ImgurDeleteUrl() const;
};

} // namespace KIPIImgurPlugin

#endif // IMGURIMAGESLIST_H
