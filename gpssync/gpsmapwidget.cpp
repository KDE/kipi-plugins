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

// Local includes.

#include "gpsmapwidget.h"
#include "gpsmapwidget.moc"

namespace KIPIGPSSyncPlugin
{

GPSMapWidget::GPSMapWidget(QWidget* parent, const QString& lat, const QString& lon, int zoomLevel)
            : KHTMLPart(parent)
{
    m_zoomLevel = QString::number(zoomLevel);
    m_latitude  = lat;
    m_longitude = lon;

    setJScriptEnabled(true);     
    setDNDEnabled(false);
    setEditable(false);

    view()->setVScrollBarMode(QScrollView::AlwaysOff);
    view()->setHScrollBarMode(QScrollView::AlwaysOff);
    view()->setMinimumSize(480, 360);
}

GPSMapWidget::~GPSMapWidget()
{
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
        m_latitude  = status.section(",", 0, 0);
        m_longitude = status.section(",", 1, 1);
        m_longitude.remove(0, 5);
        m_zoomLevel = status.section(",", 2, 2);
        m_zoomLevel.remove(0, 6);
        emit signalNewGPSLocationFromMap(m_latitude, m_longitude);
    }

    // If a new map zoom level have been selected, the Status 
    // string is like : "newZoomLevel:5"
    if (status.startsWith(QString("newZoomLevel:")))
    {
        status.remove(0, 13);
        m_zoomLevel = status;
    }
}

void GPSMapWidget::resized()
{
    QString url("http://digikam3rdparty.free.fr/gpslocator/getlonlat.php");
    url.append("?lat=");
    url.append(m_latitude);
    url.append("&lon=");
    url.append(m_longitude);
    url.append("&wth=");
    url.append(QString::number(view()->width()));
    url.append("&hgt=");
    url.append(QString::number(view()->height()));
    url.append("&zom=");
    url.append(m_zoomLevel);
    openURL(KURL(url));
    kdDebug( 51001 ) << url << endl;
}

}  // namespace KIPIGPSSyncPlugin
