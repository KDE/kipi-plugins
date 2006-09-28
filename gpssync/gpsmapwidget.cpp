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
    view()->resize(640, 480);
    setJScriptEnabled(true);       
    QString url("http://digikam3rdparty.free.fr/gpslocator/getlonlat.php");
    url.append("?lat=");
    url.append(lat);
    url.append("&lon=");
    url.append(lon);
    openURL(KURL(url));
}

GPSMapWidget::~GPSMapWidget()
{
}

void GPSMapWidget::khtmlMouseReleaseEvent(khtml::MouseReleaseEvent *)
{
    emit signalMouseReleased();
}

}  // namespace KIPIGPSSyncPlugin
