/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-28
 * Description : a widget to display a GPS web map locator.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Gerhard Kulzer <gerhard at kulzer dot net>
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

#include "gpsmapwidget.h"
#include "gpsmapwidget.moc"

// KDE includes

#include <kdebug.h>
#include <khtmlview.h>
#include <kurl.h>

namespace KIPIGPSSyncPlugin
{

class GPSMapWidgetPrivate
{

public:

    GPSMapWidgetPrivate()
    {
        gpsLocalorUrl = QString("http://digikam3rdparty.free.fr/gpslocator/getlonlatalt.php");
    }

    QString gpsLocalorUrl;
    QString latitude;
    QString longitude;
    QString altitude;
    QString zoomLevel;
    QString mapType;
    QString fileName;
};

GPSMapWidget::GPSMapWidget(QWidget* parent)
            : KHTMLPart(parent), d(new GPSMapWidgetPrivate)
{
    setJScriptEnabled(true);
    setDNDEnabled(false);
    setEditable(false);

    view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view()->setMinimumSize(480, 360);
    view()->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
}

GPSMapWidget::~GPSMapWidget()
{
    delete d;
}

void GPSMapWidget::setFileName(const QString& fileName)
{
    d->fileName = fileName;
}

QString GPSMapWidget::fileName()
{
    return d->fileName;
}

void GPSMapWidget::setGPSPosition(const QString& lat, const QString& lon)
{
    d->latitude  = lat;
    d->longitude = lon;
}

void GPSMapWidget::setMapType(const QString& mapType)
{
    d->mapType = mapType;
}

QString GPSMapWidget::mapType()
{
    return d->mapType;
}

void GPSMapWidget::setZoomLevel(int zoomLevel)
{
    d->zoomLevel = QString::number(zoomLevel);
}

int GPSMapWidget::zoomLevel()
{
    return d->zoomLevel.toInt();
}

void GPSMapWidget::extractGPSPositionfromStatusbar(const QString& txt)
{
    QString status = txt;
    status.remove(0, 5);
    status.truncate(status.length()-1);
    d->latitude  = status.section(",", 0, 0);
    d->longitude = status.section(",", 1, 1);
    d->altitude = status.section(",", 2, 2);
    d->longitude.remove(0, 5);
    d->altitude.remove(0, 5);
    emit signalNewGPSLocationFromMap(d->latitude, d->longitude, d->altitude);
}

void GPSMapWidget::khtmlMouseMoveEvent(khtml::MouseMoveEvent *e)
{
    QString status = jsStatusBarText();

    // If a new point to the map have been moved, the Status
    // string is like : "(lat:25.5894748, lon:47.6897455478, alt:211)"
    if (status.startsWith(QString("(lat:")))
        extractGPSPositionfromStatusbar(status);

    KHTMLPart::khtmlMouseMoveEvent(e);
}

void GPSMapWidget::khtmlMouseReleaseEvent(khtml::MouseReleaseEvent *e)
{
    QString status = jsStatusBarText();

    // If a new point to the map have been moved, the Status
    // string is like : "(lat:25.5894748, lon:47.6897455478, alt:211)"
    if (status.startsWith(QString("(lat:")))
        extractGPSPositionfromStatusbar(status);

    // If a new map zoom level have been selected, the Status
    // string is like : "newZoomLevel:5"
    if (status.startsWith(QString("newZoomLevel:")))
    {
        status.remove(0, 13);
        d->zoomLevel = status;
    }

    // If a new map type have been selected, the Status
    // string is like : "newMapType:G_SATELLITE_MAP"
    if (status.startsWith(QString("newMapType:")))
    {
        status.remove(0, 11);
        d->mapType = status;
    }

    KHTMLPart::khtmlMouseReleaseEvent(e);
}

void GPSMapWidget::resized()
{
    QString url = d->gpsLocalorUrl;
    url.append("?latitude=");
    url.append(d->latitude);
    url.append("&longitude=");
    url.append(d->longitude);
    url.append("&altitude=");
    url.append(d->altitude);
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
    kDebug(AREA_CODE_LOADING) << url ;
}

}  // namespace KIPIGPSSyncPlugin
