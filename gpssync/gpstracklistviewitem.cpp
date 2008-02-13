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

#include <QDateTime>
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

    QDateTime        dateTime;

    GPSTrackListItem data;
};

GPSTrackListViewItem::GPSTrackListViewItem(QTreeWidget *view)
                    : QTreeWidgetItem(view)
{
    d = new GPSTrackListViewItemPriv;
    setIcon(0, SmallIcon("image-x-generic", KIconLoader::SizeLarge, KIconLoader::DisabledState));
}

GPSTrackListViewItem::~GPSTrackListViewItem()
{
    delete d;
}

bool GPSTrackListViewItem::isDirty() const
{
    return d->data.isDirty();
}

void GPSTrackListViewItem::setGPSInfo(const QDateTime& dt, const GPSTrackListItem& data)
{
    d->dateTime = dt;
    d->data     = data;
    setText(1, QString::number(d->data.id()));
    setText(2, d->data.fileName());
    setText(3, d->dateTime.toString(Qt::LocalDate));
    setText(4, QString::number(d->data.gpsData().latitude(),  'g', 12));
    setText(5, QString::number(d->data.gpsData().longitude(), 'g', 12));
    setText(6, QString::number(d->data.gpsData().altitude(),  'g', 12));
    setText(7, isDirty() ? i18n("Yes") : i18n("No"));
}

GPSTrackListItem GPSTrackListViewItem::gpsInfo() const
{
    return d->data;
}

void GPSTrackListViewItem::setThumbnail(const QPixmap& pix)
{
    setIcon(0, pix.scaled(64, 64, Qt::KeepAspectRatio));
}

QDateTime GPSTrackListViewItem::dateTime() const
{
    return d->dateTime;
}

KUrl GPSTrackListViewItem::url() const
{
    return d->data.url();
}

QString GPSTrackListViewItem::fileName() const
{
    return d->data.fileName();
}

int GPSTrackListViewItem::id() const
{
    return d->data.id();
}

} // NameSpace KIPIGPSSyncPlugin
