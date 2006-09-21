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

class GPSListViewItemPriv
{
public:

    GPSListViewItemPriv()
    {
        enabled   = false;
        altitude  = 0.0;
        latitude  = 0.0;
        longitude = 0.0;
    }

    bool      enabled;

    double    altitude;
    double    latitude;
    double    longitude;
    
    QDateTime date;

    KURL      url;
};

GPSListViewItem::GPSListViewItem(KListView *view, QListViewItem *after, const KURL& url)
               : KListViewItem(view, after)
{
    d = new GPSListViewItemPriv;
    d->url = url;

    setEnabled(false);
    setPixmap(0, SmallIcon( "file_broken", KIcon::SizeLarge, KIcon::DisabledState ));
    setText(1, d->url.fileName());

    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.load(d->url.path());
    setDateTime(exiv2Iface.getImageDateTime());
    double alt, lat, lng;
    exiv2Iface.getGPSInfo(alt, lat, lng);
    setGPSInfo(alt, lat, lng);

    KIO::PreviewJob* thumbnailJob = KIO::filePreview(url, 64);

    connect(thumbnailJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            this, SLOT(slotGotThumbnail(const KFileItem*, const QPixmap&)));
}

GPSListViewItem::~GPSListViewItem()
{
    delete d;
}

void GPSListViewItem::setGPSInfo(double altitude, double latitude, double longitude)
{
    d->altitude  = altitude;
    d->latitude  = latitude;
    d->longitude = longitude;
    setText(2, QString::number(d->altitude));
    setText(3, QString::number(d->latitude));
    setText(4, QString::number(d->longitude));
}

void GPSListViewItem::getGPSInfo(double& altitude, double& latitude, double& longitude)
{
    if (isEnabled())
    {
        altitude  = d->altitude;
        latitude  = d->latitude;
        longitude = d->longitude;
    }
}

void GPSListViewItem::setDateTime(QDateTime date)
{
    if (date.isValid())
    {
        setEnabled(true);
        d->date = date;
        setText(5, date.toString(Qt::ISODate));
    }
    else
    {
        setText(5, i18n("Not available"));
    }
}

QDateTime GPSListViewItem::getDateTime()
{
    return d->date;
}

KURL GPSListViewItem::getUrl()
{
    return d->url;
}

void GPSListViewItem::writeGPSInfoToFile()
{
    if (isEnabled())
    {
        setPixmap(1, SmallIcon("run"));
        KIPIPlugins::Exiv2Iface exiv2Iface;
        exiv2Iface.load(d->url.path());
        bool ret = exiv2Iface.setGPSInfo(d->altitude, d->latitude, d->longitude);
        ret &= exiv2Iface.save(d->url.path());
        if (ret)
            setPixmap(1, SmallIcon("ok"));
        else
            setPixmap(1, SmallIcon("cancel"));
    }
}

void GPSListViewItem::setEnabled(bool e)    
{
    d->enabled = e;
    repaint();
}

bool GPSListViewItem::isEnabled(void)    
{
    return d->enabled;
}

void GPSListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    if (d->enabled)
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
    repaint();
}

} // NameSpace KIPIGPSSyncPlugin

