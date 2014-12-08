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

#include "piimglist.h"
#include "piimglist.moc"

// Qt includes

#include <QLabel>
#include <QDesktopServices>

// KDE includes

#include <kdebug.h>
#include <klocale.h>

// Local includes

#include "kpmetadata.h"

namespace KIPIPhotivoIntegrationPlugin
{

// PIImgList public ///////////////////////////////////////////////////////////

PIImgList::PIImgList(QWidget* const parent)
    : KPImagesList(parent)
{
    setControlButtonsPlacement(KPImagesList::NoControlButtons);
    setAllowDuplicate(false);
    setAllowRAW(true);

    listView()->setColumn(static_cast<KPImagesListView::ColumnType>(PIImgList::Description),
                          i18n("..."),
                          true
                         );//TODO
}

// ----------------------------------------------------------------------------

PIImgList::~PIImgList()
{
}

// PIImgListViewItem public ///////////////////////////////////////////////////

PIImgListViewItem::PIImgListViewItem(KPImagesListView* const view, const KUrl& url)
    : KPImagesListViewItem(view, url)
{
}

// ----------------------------------------------------------------------------

PIImgListViewItem::~PIImgListViewItem()
{
}

// ----------------------------------------------------------------------------

void PIImgListViewItem::setTitle(const QString& str)
{
    setText(PIImgList::Title, str);
    m_Title = str;
}

QString PIImgListViewItem::Title() const
{
    return m_Title;
}

// ----------------------------------------------------------------------------

void PIImgListViewItem::setDescription(const QString& str)
{
    setText(PIImgList::Description, str);
    m_Description = str;
}

QString PIImgListViewItem::Description() const
{
    return m_Description;
}

// ----------------------------------------------------------------------------

} // namespace KIPIPhotivoIntegrationPlugin
