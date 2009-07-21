/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-07
 * Description : GPS track list view item.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef GPSTRACKLISTVIEWITEM_H
#define GPSTRACKLISTVIEWITEM_H

// Qt includes

#include <QDateTime>
#include <QString>

// KDE includes

#include <kurl.h>

// Local includes

#include "gpstracklistcontainer.h"
#include "imageslist.h"

namespace KIPIGPSSyncPlugin
{
class GPSTrackListViewItemPriv;

class GPSTrackListViewItem : public KIPIPlugins::ImagesListViewItem
{

public:

    GPSTrackListViewItem(KIPIPlugins::ImagesListView *view, const KUrl& url);
    ~GPSTrackListViewItem();

    void setGPSInfo(const QDateTime& dt, const GPSTrackListItem& data);
    GPSTrackListItem gpsInfo() const;

    void setDirty(bool dirty);
    bool isDirty() const;

    int              id() const;
    QDateTime        dateTime() const;
    QString          fileName() const;
    KUrl             url() const;

private:

    GPSTrackListViewItemPriv* const d;
};

} // namespace KIPIGPSSyncPlugin

#endif /* GPSTRACKLISTVIEWITEM_H */
