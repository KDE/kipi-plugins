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
        gpsTrackListUrl = QString("http://digikam3rdparty.free.fr/gpslocator/tracklistedit.php");
    }

    QString      gpsTrackListUrl;
    QString      zoomLevel;
    QString      mapType;

    GPSTrackList gpsTrackList;
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

void GPSTrackListWidget::setTrackList(const GPSTrackList& gpsTrackList)
{
    d->gpsTrackList = gpsTrackList;
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
    // string is like : "(mkr:1, lat:25.5894748, lon:47.6897455478)"
    if (status.startsWith(QString("(mkr:")))
    {
        status.remove(0, 5);
        status.truncate(status.length()-1);
        QString id        = status.section(",", 0, 0);
        QString latitude  = status.section(",", 1, 1);
        latitude.remove(0, 5);
        QString longitude = status.section(",", 2, 2);
        longitude.remove(0, 6);
        emit signalNewGPSLocationFromMap(id, latitude, longitude);
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
    QString url = d->gpsTrackListUrl;
    url.append("?width=");
    url.append(QString::number(view()->width()));
    url.append("&height=");
    url.append(QString::number(view()->height()));
    url.append("&zoom=");
    url.append(d->zoomLevel);
    url.append("&maptype=");
    url.append(d->mapType);

    int count = d->gpsTrackList.count();
    url.append("&items=");
    url.append(QString::number(count));
    for( GPSTrackList::iterator it = d->gpsTrackList.begin() ; 
         it != d->gpsTrackList.end() ; ++it)
    {
        int id = it.value().id();
        url.append(QString("&lat%1=").arg(id));
        url.append(QString::number(it.value().gpsData().latitude()));
        url.append(QString("&lng%1=").arg(id));
        url.append(QString::number(it.value().gpsData().longitude()));
        url.append(QString("&title%1=").arg(id));
        url.append(QString("%1").arg(id));
    }

    openUrl(KUrl(url));
    kDebug( 51001 ) << url << endl;
}

}  // namespace KIPIGPSSyncPlugin
