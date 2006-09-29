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

GPSMapWidget::GPSMapWidget(QWidget* parent, const QString& lat, const QString& lon)
            : KHTMLPart(parent)
{
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
    
    if (status.startsWith(QString("(lat:")))
    {
        status.remove(0, 5);
        status.truncate(status.length()-1);
        m_latitude  = status.section(",", 0, 0);
        m_longitude = status.section(",", 1, 1);
        m_longitude.remove(0, 5);
        emit signalNewGPSLocationFromMap(m_latitude, m_longitude);
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
    openURL(KURL(url));
    kdDebug( 51001 ) << url << endl;
}

}  // namespace KIPIGPSSyncPlugin
