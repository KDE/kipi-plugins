/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-07
 * Description : GPS track list view item.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QPainter>
#include <QFileInfo>

// KDE includes.

#include <klocale.h>
#include <kiconloader.h>

// Local includes.

#include "gpstracklistviewitem.h"

namespace KIPIGPSSyncPlugin
{

class GPSTrackListViewItemPriv
{
public:

    GPSTrackListViewItemPriv()
    {
    }

    GPSTrackListItem gpsData;
};

GPSTrackListViewItem::GPSTrackListViewItem(K3ListView *view, Q3ListViewItem *after)
                    : K3ListViewItem(view, after)
{
    d = new GPSTrackListViewItemPriv;
    setPixmap(0, SmallIcon("file_broken", KIconLoader::SizeLarge, KIconLoader::DisabledState));
}

GPSTrackListViewItem::~GPSTrackListViewItem()
{
    delete d;
}

void GPSTrackListViewItem::setGPSInfo(const GPSTrackListItem& gpsData)
{
    d->gpsData = gpsData;
    setText(1, d->gpsData.fileName());
    setText(2, QString::number(d->gpsData.id()));
}

GPSTrackListItem GPSTrackListViewItem::GPSInfo() const
{
    return d->gpsData;
}

void GPSTrackListViewItem::setThumbnail(const QPixmap& pix)
{
    setPixmap(0, pix.scaled(64, 64, Qt::KeepAspectRatio));
}

QDateTime GPSTrackListViewItem::dateTime() const
{
    return d->gpsData.dateTime();
}

KUrl GPSTrackListViewItem::url() const
{
    return d->gpsData.url();
}

QString GPSTrackListViewItem::fileName() const
{
    return d->gpsData.fileName();
}

int GPSTrackListViewItem::id() const
{
    return d->gpsData.id();
}

} // NameSpace KIPIGPSSyncPlugin
