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
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

// Qt includes

#include <QTimer>

// KDE includes

#include <kdebug.h>
#include <khtmlview.h>
#include <kurl.h>

// local includes

#include "pluginsversion.h"

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
    int     apiVersion;
    QString altitudeService;
    QString extraOptions;

    QString lastKHTMLStatus;
    QTimer* statusTimer;
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

    d->statusTimer = new QTimer(this);
    d->statusTimer->setSingleShot(false);
    d->statusTimer->start(100);

    connect(d->statusTimer, SIGNAL(timeout()),
            this, SLOT(slotReadKHTMLStatus()));
}

GPSMapWidget::~GPSMapWidget()
{
    delete d;
}

void GPSMapWidget::setExtraOptions(const QString& extraOptions)
{
    d->extraOptions = extraOptions;
}

void GPSMapWidget::setApiVersion(const int apiVersion)
{
    d->apiVersion = apiVersion;
}

int GPSMapWidget::apiVersion() const
{
    return d->apiVersion;
}

void GPSMapWidget::setAltitudeService(const QString& altitudeService)
{
    d->altitudeService = altitudeService;
}

QString GPSMapWidget::altitudeService() const
{
    return d->altitudeService;
}

void GPSMapWidget::setFileName(const QString& fileName)
{
    d->fileName = fileName;
}

QString GPSMapWidget::fileName() const
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

QString GPSMapWidget::mapType() const
{
    return d->mapType;
}

void GPSMapWidget::setZoomLevel(int zoomLevel)
{
    d->zoomLevel = QString::number(zoomLevel);
}

int GPSMapWidget::zoomLevel() const
{
    return d->zoomLevel.toInt();
}

void GPSMapWidget::extractGPSPositionfromStatusbar(const QString& txt)
{
    QString status = txt;
    status.remove(0, 5);
    status.truncate(status.length()-1);
    d->latitude  = status.section(',', 0, 0);
    d->longitude = status.section(',', 1, 1);
    d->altitude = status.section(',', 2, 2);
    d->longitude.remove(0, 5);
    d->altitude.remove(0, 5);
    emit signalNewGPSLocationFromMap(d->latitude, d->longitude, d->altitude);
}

void GPSMapWidget::khtmlMouseMoveEvent(khtml::MouseMoveEvent *e)
{
    slotReadKHTMLStatus();

    KHTMLPart::khtmlMouseMoveEvent(e);
}

void GPSMapWidget::khtmlMouseReleaseEvent(khtml::MouseReleaseEvent *e)
{
    slotReadKHTMLStatus();

    KHTMLPart::khtmlMouseReleaseEvent(e);
}

void GPSMapWidget::resized()
{
    KUrl url = d->gpsLocalorUrl;
    url.addQueryItem("latitude", d->latitude);
    url.addQueryItem("longitude", d->longitude);
    url.addQueryItem("altitude", d->altitude);
    url.addQueryItem("width", QString::number(view()->width()));
    url.addQueryItem("height", QString::number(view()->height()));
    url.addQueryItem("zoom", d->zoomLevel);
    url.addQueryItem("maptype", d->mapType);
    url.addQueryItem("filename", d->fileName);
    url.addQueryItem("altitudeservice", d->altitudeService);
    url.addQueryItem("gmapsversion", QString::number(d->apiVersion));
    url.addQueryItem("pluginversion", QString(kipiplugins_version));
    url.addQueryItem("extraoptions", d->extraOptions);
    url.addQueryItem("maplang", 
                    i18nc(
                        "Language code for the embedded Google Maps. "
                        "Please take a look at "
                        "http://spreadsheets.google.com/pub?key=p9pdwsai2hDMsLkXsoM05KQ&gid=1 "
                        "for supported languages. If your language is not on the list, pick 'en'."
                        , "en"));
    openUrl(url);
    kDebug(AREA_CODE_LOADING) << url;
}

void GPSMapWidget::slotReadKHTMLStatus()
{
    QString status = jsStatusBarText();

    // did the status change?
    if (status==d->lastKHTMLStatus)
        return;

    kDebug()<<status;
    d->lastKHTMLStatus = status;

    // If a marker has been moved to another point on the map, the status
    // string is like : "(lat:25.5894748, lon:47.6897455478, alt:211)"
    if (status.startsWith(QString("(lat:")))
    {
        extractGPSPositionfromStatusbar(status);
    }

    // If a new map zoom level has been selected, the status
    // string is like : "newZoomLevel:5"
    if (status.startsWith(QString("newZoomLevel:")))
    {
        status.remove(0, 13);
        d->zoomLevel = status;
    }

    // If a new map type has been selected, the status
    // string is like : "newMapType:G_SATELLITE_MAP"
    if (status.startsWith(QString("newMapType:")))
    {
        status.remove(0, 11);
        d->mapType = status;
    }
}

}  // namespace KIPIGPSSyncPlugin
