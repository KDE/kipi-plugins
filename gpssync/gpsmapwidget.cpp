/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-28
 * Description : a widget to display a GPS web map locator.
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// KDE includes.

#include <kdebug.h>
#include <khtmlview.h>
#include <kurl.h>
#include <kconfig.h>
#include <dcopref.h>

// Local includes.

#include "gpsmapwidget.h"
#include "gpsmapwidget.moc"

namespace KIPIGPSSyncPlugin
{

class GPSMapWidgetPrivate
{

public:

    GPSMapWidgetPrivate()
    {
        gpsLocalorUrl = QString("http://digikam3rdparty.free.fr/gpslocator/getlonlat.php");
    }

    QString gpsLocalorUrl;
    QString latitude;
    QString longitude;
    QString zoomLevel;
};

GPSMapWidget::GPSMapWidget(QWidget* parent, const QString& lat, const QString& lon, int zoomLevel)
            : KHTMLPart(parent)
{
    d = new GPSMapWidgetPrivate;
    d->zoomLevel = QString::number(zoomLevel);
    d->latitude  = lat;
    d->longitude = lon;

    setJScriptEnabled(true);
    setDNDEnabled(false);
    setEditable(false);

    view()->setVScrollBarMode(QScrollView::AlwaysOff);
    view()->setHScrollBarMode(QScrollView::AlwaysOff);
    view()->setMinimumSize(480, 360);

    // We will force KHTMLPart to use a safary browser identification with
    // the GPS locator url. This is mandatory because Google Maps do not yet
    // support konqueror identification.

    KConfig config("kio_httprc");
    config.setGroup(d->gpsLocalorUrl.lower());
    config.writeEntry("UserAgent", "Mozilla/5.0 (Macintosh; U; PPC Mac OS X; appLanguage) AppleWebKit/125.5.5 (KHTML, like Gecko) Safari/125.11");
    config.sync();

    // Inform running http(s) io-slaves about the browser id. change...
    if (!DCOPRef("*", "KIO::Scheduler").send("reparseSlaveConfiguration", QString::null))
        kdWarning() << "Unable to dispatch browser id change to http io-slaves" 
                    << endl;
}

GPSMapWidget::~GPSMapWidget()
{
    delete d;
}

void GPSMapWidget::khtmlMouseReleaseEvent(khtml::MouseReleaseEvent *)
{
    QString status = jsStatusBarText();
    
    // If a new point to the map have been selected, the Status 
    // string is like : "(lat:25.5894748, lon:47.6897455478, zoom:8)"
    if (status.startsWith(QString("(lat:")))
    {
        status.remove(0, 5);
        status.truncate(status.length()-1);
        d->latitude  = status.section(",", 0, 0);
        d->longitude = status.section(",", 1, 1);
        d->longitude.remove(0, 5);
        d->zoomLevel = status.section(",", 2, 2);
        d->zoomLevel.remove(0, 6);
        emit signalNewGPSLocationFromMap(d->latitude, d->longitude);
    }

    // If a new map zoom level have been selected, the Status 
    // string is like : "newZoomLevel:5"
    if (status.startsWith(QString("newZoomLevel:")))
    {
        status.remove(0, 13);
        d->zoomLevel = status;
    }
}

void GPSMapWidget::resized()
{
    QString url = d->gpsLocalorUrl;
    url.append("?lat=");
    url.append(d->latitude);
    url.append("&lon=");
    url.append(d->longitude);
    url.append("&wth=");
    url.append(QString::number(view()->width()));
    url.append("&hgt=");
    url.append(QString::number(view()->height()));
    url.append("&zom=");
    url.append(d->zoomLevel);
    openURL(KURL(url));
    kdDebug( 51001 ) << url << endl;
}

}  // namespace KIPIGPSSyncPlugin
