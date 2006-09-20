/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-19
 * Description : GPS file list view item.
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

// Qt includes.

#include <qstring.h>
#include <qpainter.h>

// KDE includes.

#include <klocale.h>
#include <kio/previewjob.h>
#include <kiconloader.h>

// Local includes.

#include "exiv2iface.h"
#include "gpslistviewitem.h"
#include "gpslistviewitem.moc"

namespace KIPIGPSSyncPlugin
{

GPSListViewItem::GPSListViewItem(KListView *view, QListViewItem *after, const KURL& url)
               : KListViewItem(view, after)
{
    m_altitude  = 0.0;
    m_latitude  = 0.0;
    m_longitude = 0.0;
    m_url       = url;

    setEnabled(false);
    setText(0, m_url.fileName());
    setPixmap(0, SmallIcon( "file_broken", KIcon::SizeLarge, KIcon::DisabledState ));

    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.load(m_url.path());
    setDateTime(exiv2Iface.getImageDateTime());
    double alt, lat, lng;
    exiv2Iface.getGPSInfo(alt, lat, lng);
    setGPSInfo(alt, lat, lng);

    KIO::PreviewJob* thumbnailJob = KIO::filePreview(url, 64);

    connect(thumbnailJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            this, SLOT(slotGotThumbnail(const KFileItem*, const QPixmap&)));
}

void GPSListViewItem::setGPSInfo(double altitude, double latitude, double longitude)
{
    m_altitude  = altitude;
    m_latitude  = latitude;
    m_longitude = longitude;
    setText(1, QString::number(m_altitude));
    setText(2, QString::number(m_latitude));
    setText(3, QString::number(m_longitude));
}

void GPSListViewItem::getGPSInfo(double& altitude, double& latitude, double& longitude)
{
    if (isEnabled())
    {
        altitude  = m_altitude;
        latitude  = m_latitude;
        longitude = m_longitude;
    }
}

void GPSListViewItem::setDateTime(QDateTime date)
{
    if (date.isValid())
    {
        setEnabled(true);
        m_date = date;
        setText(4, date.toString(Qt::ISODate));
    }
    else
    {
        setText(4, i18n("Not available"));
    }
}

QDateTime GPSListViewItem::getDateTime()
{
    return m_date;
}

KURL GPSListViewItem::getUrl()
{
    return m_url;
}

void GPSListViewItem::writeGPSInfoToFile()
{
    if (isEnabled())
    {
        KIPIPlugins::Exiv2Iface exiv2Iface;
        exiv2Iface.load(m_url.path());
        bool ret = exiv2Iface.setGPSInfo(m_altitude, m_latitude, m_longitude);
        ret &= exiv2Iface.save(m_url.path());
        if (ret)
            setPixmap(4, SmallIcon("ok"));
        else
            setPixmap(4, SmallIcon("cancel"));
    }
}

void GPSListViewItem::setEnabled(bool d)    
{
    m_enabled = d;
    repaint();
}

bool GPSListViewItem::isEnabled(void)    
{
    return m_enabled;
}

void GPSListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    if (m_enabled)
    {
        KListViewItem::paintCell(p, cg, column, width, alignment);
    }
    else
    {
        QColorGroup _cg( cg );
        QColor c = _cg.text();
        _cg.setColor( QColorGroup::Text, Qt::gray );
        KListViewItem::paintCell( p, _cg, column, width, alignment );
        _cg.setColor( QColorGroup::Text, c );
    }
}

void GPSListViewItem::slotGotThumbnail(const KFileItem*, const QPixmap& pix)
{
    setPixmap(0, pix);
}

} // NameSpace KIPIGPSSyncPlugin

