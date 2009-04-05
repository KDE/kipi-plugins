/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-19
 * Description : GPS file list view item.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef GPSLISTVIEWITEM_H
#define GPSLISTVIEWITEM_H

// Qt includes

#include <QDateTime>

// KDE includes

#include <kurl.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "gpsdatacontainer.h"
#include "imageslist.h"

namespace KIPIGPSSyncPlugin
{
class GPSListViewItemPriv;

class GPSListViewItem : public KIPIPlugins::ImagesListViewItem
{

public:

    GPSListViewItem(KIPI::Interface* interface, KIPIPlugins::ImagesListView *view, const KUrl& url);
    ~GPSListViewItem();

    void setGPSInfo(const GPSDataContainer& gpsData, bool dirty=true, bool addedManually=false);
    GPSDataContainer GPSInfo() const;
    void eraseGPSInfo();

    void setDateTime(const QDateTime& date);
    QDateTime dateTime() const;

    void setEnabled(bool e);
    bool isEnabled();
    bool isDirty();
    bool isInterpolated();
    bool hasGPSInfo();

    KUrl url() const;
    void writeGPSInfoToFile();

    void setThumbnail(const QPixmap& pix);

private:

    GPSListViewItemPriv* const d;
};

} // namespace KIPIGPSSyncPlugin

#endif /* GPSLISTVIEWITEM_H */
