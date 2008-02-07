/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-08
 * Description : a widget to display a GPS track list over web map locator.
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

// KDE includes.

#include <kdebug.h>
#include <khtmlview.h>
#include <kurl.h>

// Local includes.

#include "gpstracklistwidget.h"
#include "gpstracklistwidget.moc"

namespace KIPIGPSSyncPlugin
{

class GPSTrackListWidgetPrivate
{

public:

    GPSTrackListWidgetPrivate()
    {
        gpsLocalorUrl = QString("http://digikam3rdparty.free.fr/gpslocator/tracklistedit.php");
    }

    QString gpsLocalorUrl;
    QString latitude;
    QString longitude;
    QString zoomLevel;
    QString mapType;
    QString fileName;
};

GPSTrackListWidget::GPSTrackListWidget(QWidget* parent)
                  : KHTMLPart(parent)
{
    d = new GPSTrackListWidgetPrivate;

    setJScriptEnabled(true);
    setDNDEnabled(false);
    setEditable(false);

    view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view()->setMinimumSize(480, 360);
    view()->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
}

GPSTrackListWidget::~GPSTrackListWidget()
{
    delete d;
}

void GPSTrackListWidget::setFileName(const QString& fileName)
{
    d->fileName = fileName;
}

QString GPSTrackListWidget::fileName()
{
    return d->fileName;
}

void GPSTrackListWidget::setGPSPosition(const QString& lat, const QString& lon)
{
    d->latitude  = lat;
    d->longitude = lon;
}

void GPSTrackListWidget::GPSPosition(QString& lat, QString& lon)
{
    lat = d->latitude;
    lon = d->longitude;
}

void GPSTrackListWidget::setMapType(const QString& mapType)
{
    d->mapType = mapType;
}

QString GPSTrackListWidget::mapType()
{
    return d->mapType;
}

void GPSTrackListWidget::setZoomLevel(int zoomLevel)
{
    d->zoomLevel = QString::number(zoomLevel);
}

int GPSTrackListWidget::GPSTrackListWidget::zoomLevel()
{
    return d->zoomLevel.toInt();
}

void GPSTrackListWidget::khtmlMouseReleaseEvent(khtml::MouseReleaseEvent *e)
{
    QString status = jsStatusBarText();

    // If a new point to the map have been moved, the Status 
    // string is like : "(lat:25.5894748, lon:47.6897455478)"
    if (status.startsWith(QString("(lat:")))
    {
        status.remove(0, 5);
        status.truncate(status.length()-1);
        d->latitude  = status.section(",", 0, 0);
        d->longitude = status.section(",", 1, 1);
        d->longitude.remove(0, 5);
        emit signalNewGPSLocationFromMap(d->latitude, d->longitude);
    }

    // If a new map zoom level have been selected, the Status 
    // string is like : "newZoomLevel:5"
    if (status.startsWith(QString("newZoomLevel:")))
    {
        status.remove(0, 13);
        d->zoomLevel = status;
    }

    // If a new map type have been selected, the Status 
    // string is like : "newMapType:G_SATELLITE_TYPE"
    if (status.startsWith(QString("newMapType:")))
    {
        status.remove(0, 11);
        d->mapType = status;
    }

    KHTMLPart::khtmlMouseReleaseEvent(e);
}

void GPSTrackListWidget::resized()
{
    QString url = d->gpsLocalorUrl;
    url.append("?latitude=");
    url.append(d->latitude);
    url.append("&longitude=");
    url.append(d->longitude);
    url.append("&width=");
    url.append(QString::number(view()->width()));
    url.append("&height=");
    url.append(QString::number(view()->height()));
    url.append("&zoom=");
    url.append(d->zoomLevel);
    url.append("&maptype=");
    url.append(d->mapType);
    url.append("&filename=");
    url.append(d->fileName);
    openUrl(KUrl(url));
    kDebug( 51001 ) << url << endl;
}

}  // namespace KIPIGPSSyncPlugin
