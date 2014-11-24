/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-17-04
 * Description : time adjust images list.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2012-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Local includes

#include "actionthread.h"

namespace KIPITimeAdjustPlugin
{

MyImageList::MyImageList(QWidget* const parent)
    : KPImagesList(parent)
{
    setControlButtonsPlacement(KPImagesList::NoControlButtons);
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(TIMESTAMP_USED),
                          i18n("Timestamp Used"), true);
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(TIMESTAMP_UPDATED),
                          i18n("Timestamp Updated"), true);
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(TIMESTAMP_FILENAME),
                          i18n("New Filename"), true);
    listView()->setColumn(static_cast<KIPIPlugins::KPImagesListView::ColumnType>(STATUS),
                          i18n("Status"), true);
}

MyImageList::~MyImageList()
{
}

void MyImageList::setItemDates(const QMap<KUrl, QDateTime>& map, FieldType type, const TimeAdjustSettings& settings)
{
    foreach (const KUrl& url, map.keys())
    {
        KPImagesListViewItem* const item = listView()->findItem(url);

        if (item)
        {
            QDateTime dt = map.value(url);

            if (dt.isValid())
            {
                item->setText(type, KGlobal::locale()->formatDateTime(dt, KLocale::ShortDate, true));
            }
            else
            {
                item->setText(type, i18n("not valid"));
            }

            if (type == TIMESTAMP_UPDATED)
            {
                if (settings.updFileName)
                    item->setText(TIMESTAMP_FILENAME, ActionThread::newUrl(url, dt).fileName());
                else
                    item->setText(TIMESTAMP_FILENAME, i18nc("not applicable", "N.A"));
            }
        }
    }
}

void MyImageList::setStatus(const QMap<KUrl, int>& status)
{
    foreach (const KUrl& url, status.keys())
    {
        KPImagesListViewItem* const item = listView()->findItem(url);

        if (item)
        {
            QStringList errors;
            int         flags = status.value(url);

            if (flags & META_TIME_ERROR)
            {
                errors << i18n("Failed to update metadata timestamp");
            }

            if (flags & FILE_TIME_ERROR)
            {
                errors << i18n("Failed to update file timestamp");
            }

            if (flags & FILE_NAME_ERROR)
            {
                errors << i18n("Failed to rename file");
            }

            if (errors.isEmpty())
            {
                item->setText(STATUS, i18n("Processed without error"));
            }
            else
            {
                item->setText(STATUS, errors.join(" | "));
            }
        }
    }
}

}  // namespace KIPITimeAdjustPlugin
