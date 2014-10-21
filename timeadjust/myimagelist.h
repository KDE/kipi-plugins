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

#ifndef MYIMAGELIST_H
#define MYIMAGELIST_H

// Qt includes

#include <QMap>
#include <QDateTime>

// KDE includes

#include <kurl.h>

// Local includes

#include "kpimageslist.h"
#include "timeadjustsettings.h"

using namespace KIPIPlugins;

namespace KIPITimeAdjustPlugin
{

class MyImageList : public KPImagesList
{
    Q_OBJECT

public:

    /* The different columns in a list. */
    enum FieldType
    {
        TIMESTAMP_USED     = KPImagesListView::User1,
        TIMESTAMP_UPDATED  = KPImagesListView::User2,
        TIMESTAMP_FILENAME = KPImagesListView::User3,
        STATUS             = KPImagesListView::User4
    };

    enum ProcessingStatus
    {
        NOPROCESS_ERROR = 1 << 0,
        META_TIME_ERROR = 1 << 1,
        FILE_TIME_ERROR = 1 << 2,
        FILE_NAME_ERROR = 1 << 3
    };

public:

    explicit MyImageList(QWidget* const parent);
    ~MyImageList();

    void setItemDates(const QMap<KUrl, QDateTime>& map, FieldType type, const TimeAdjustSettings& settings);
    void setStatus(const QMap<KUrl, int>& status);
};
}  // namespace KIPITimeAdjustPlugin

#endif // MYIMAGELIST_H
