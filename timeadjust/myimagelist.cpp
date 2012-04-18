/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-17-04
 * Description : time adjust images list.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "myimagelist.moc"

// KDE includes

#include <klocale.h>

namespace KIPITimeAdjustPlugin
{

MyImageList::MyImageList(QWidget* const parent)
    : KPImagesList(parent)
{
    setControlButtonsPlacement(KPImagesList::NoControlButtons);
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(TIMESTAMPUSED),
                          i18n("Timestamp Used"), true);
}

MyImageList::~MyImageList()
{
}

void MyImageList::setItemOriginalDates(const QMap<KUrl, QDateTime>& map)
{
    foreach (const KUrl& url, map.keys())
    {
        KPImagesListViewItem* item = listView()->findItem(url);
        if (item)
        {
            QDateTime dt = map.value(url);
            if (dt.isValid())
            {
                item->setText(TIMESTAMPUSED, dt.toString(Qt::ISODate));
            }
            else
            {
                item->setText(TIMESTAMPUSED, i18n("not valid"));
            }
        }
    }
}

}  // namespace KIPITimeAdjustPlugin
