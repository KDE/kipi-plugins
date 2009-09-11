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

#include "gpslistviewitem.h"

// KDE includes

#include <klocale.h>
#include <kiconloader.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

namespace KIPIGPSSyncPlugin
{

class GPSListViewItemPriv
{
public:

    GPSListViewItemPriv()
    {
        enabled    = false;
        dirty      = false;
        erase      = false;
        hasGPSInfo = false;
        interface  = 0;
    }

    bool             enabled;
    bool             dirty;
    bool             erase;
    bool             hasGPSInfo;

    QDateTime        date;

    KUrl             url;

    KIPI::Interface *interface;

    GPSDataContainer gpsData;
};

GPSListViewItem::GPSListViewItem(KIPI::Interface* interface, KIPIPlugins::ImagesListView *view, const KUrl& url)
               : KIPIPlugins::ImagesListViewItem(view, url),
                 d(new GPSListViewItemPriv)
{
    d->interface = interface;
    d->url       = url;

    setEnabled(false);

    double alt, lat, lng;
    KExiv2Iface::KExiv2 exiv2Iface;
    QMap<QString, QVariant> attributes;
    KIPI::ImageInfo info = d->interface->info(d->url);
    attributes = info.attributes();

    if (attributes.contains("latitude") &&
        attributes.contains("longitude") &&
        attributes.contains("altitude"))
    {
        lat = attributes["latitude"].toDouble();
        lng = attributes["longitude"].toDouble();
        alt = attributes["altitude"].toDouble();
        d->hasGPSInfo = true;
    }
    else
    {
        exiv2Iface.load(d->url.path());
        d->hasGPSInfo = exiv2Iface.getGPSInfo(alt, lat, lng);
    }

    if (hasGPSInfo())
        setGPSInfo(GPSDataContainer(alt, lat, lng, false), false);

    QDateTime dt = info.time(KIPI::FromInfo);
    if (!dt.isValid())
        dt = exiv2Iface.getImageDateTime();

    if (dt.isValid())
        setDateTime(dt);
}

GPSListViewItem::~GPSListViewItem()
{
    delete d;
}

void GPSListViewItem::setGPSInfo(const GPSDataContainer& gpsData, bool dirty, bool addedManually)
{
    setEnabled(true);
    d->dirty      = dirty;
    d->gpsData    = gpsData;
    d->erase      = false;
    d->hasGPSInfo = true;
    setText(KIPIPlugins::ImagesListView::User2, QString::number(d->gpsData.latitude(),  'g', 12));
    setText(KIPIPlugins::ImagesListView::User3, QString::number(d->gpsData.longitude(), 'g', 12));
    setText(KIPIPlugins::ImagesListView::User4, QString::number(d->gpsData.altitude(),  'g', 12));

    if (isDirty())
    {
        QString status;

        if (d->gpsData.isInterpolated())
            status = i18n("Interpolated");
        else
        {
            if (addedManually)
                status = i18n("Added");
            else
                status = i18n("Found");
        }

        setText(KIPIPlugins::ImagesListView::User5, status);
    }
}

GPSDataContainer GPSListViewItem::GPSInfo() const
{
    return d->gpsData;
}

void GPSListViewItem::eraseGPSInfo()
{
    d->erase = true;
    d->dirty = true;
    setText(KIPIPlugins::ImagesListView::User5, i18n("Deleted"));
}

void GPSListViewItem::setDateTime(const QDateTime& date)
{
    if (date.isValid())
    {
        d->date = date;
        setText(KIPIPlugins::ImagesListView::User1, date.toString(Qt::LocalDate));
    }
    else
    {
        setText(KIPIPlugins::ImagesListView::User1, i18n("Not available"));
    }
}

QDateTime GPSListViewItem::dateTime() const
{
    return d->date;
}

KUrl GPSListViewItem::url() const
{
    return d->url;
}

bool GPSListViewItem::hasGPSInfo()
{
    return d->hasGPSInfo;
}

bool GPSListViewItem::isInterpolated()
{
    return d->gpsData.isInterpolated();
}

void GPSListViewItem::writeGPSInfoToFile()
{
    QMap<QString, QVariant> attributes;

    if (isEnabled() && isDirty())
    {
        KExiv2Iface::KExiv2 exiv2Iface;
        exiv2Iface.setWriteRawFiles(d->interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
        exiv2Iface.setUpdateFileTimeStamp(d->interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

        // TODO: there is no error-checking here
        //       We should warn the user if writing to the
        //       file fails!
        exiv2Iface.load(d->url.path());

        KIPI::ImageInfo info = d->interface->info(url());

        if (d->erase)
        {
            // Remove file metadata GPS location.
            exiv2Iface.removeGPSInfo();

            // Remove kipi host GPS location
            QStringList list;
            // note: delAttributes wants "gpslocation", addAttributes 
            //       wants "latitude", "longitude" and "altitude"
            list << "gpslocation";
            info.delAttributes(list);
        }
        else
        {
            // Set file metadata GPS location.
            exiv2Iface.setGPSInfo(d->gpsData.altitude(),
                                  d->gpsData.latitude(),
                                  d->gpsData.longitude());

            // Set kipi host GPS location
            attributes.clear();
            attributes.insert("latitude",  d->gpsData.latitude());
            attributes.insert("longitude", d->gpsData.longitude());
            attributes.insert("altitude",  d->gpsData.altitude());
            info.addAttributes(attributes);
        }

        exiv2Iface.save(d->url.path());

        d->dirty = false;
    }
}

void GPSListViewItem::setEnabled(bool e)
{
    d->enabled = e;
}

bool GPSListViewItem::isEnabled()
{
    return d->enabled;
}

bool GPSListViewItem::isDirty()
{
    return d->dirty;
}

void GPSListViewItem::setThumbnail(const QPixmap& pix)
{
    setIcon(KIPIPlugins::ImagesListView::Thumbnail,
            pix.scaled(64, 64, Qt::KeepAspectRatio));
}

} // namespace KIPIGPSSyncPlugin
