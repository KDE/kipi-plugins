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
        enabled = false;
        dirty   = false;
        erase   = false;
    }

    bool             enabled;
    bool             dirty;
    bool             erase;

    QDateTime        date;

    KURL             url;

    GPSDataContainer gpsData;    
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
    if (exiv2Iface.getGPSInfo(alt, lat, lng))
        setGPSInfo(GPSDataContainer(alt, lat, lng, false), false);

    KIO::PreviewJob* thumbnailJob = KIO::filePreview(url, 64);

    connect(thumbnailJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            this, SLOT(slotGotThumbnail(const KFileItem*, const QPixmap&)));
}

GPSListViewItem::~GPSListViewItem()
{
    delete d;
}

void GPSListViewItem::setGPSInfo(GPSDataContainer gpsData, bool dirty, bool addedManually)
{
    setEnabled(true);
    d->dirty   = dirty;
    d->gpsData = gpsData;
    setText(2, QString::number(d->gpsData.altitude(),  'g', 12));
    setText(3, QString::number(d->gpsData.latitude(),  'g', 12));
    setText(4, QString::number(d->gpsData.longitude(), 'g', 12));

    QString status;
    if (isDirty())
    {
        if (d->gpsData.isInterpolated())
            status = i18n("Interpolated");
        else
        {
            if (addedManually)
                status = i18n("Added");
            else
                status = i18n("Found");
        }
    }
    setText(6, status);

    repaint();
}

GPSDataContainer GPSListViewItem::getGPSInfo()
{
    return d->gpsData;
}

void GPSListViewItem::setDateTime(QDateTime date)
{
    if (date.isValid())
    {
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

bool GPSListViewItem::isInterpolated()
{
    return d->gpsData.isInterpolated();
}

void GPSListViewItem::writeGPSInfoToFile()
{
    if (isEnabled() && isDirty())
    {
        setPixmap(1, SmallIcon("run"));
        KIPIPlugins::Exiv2Iface exiv2Iface;
        bool ret = exiv2Iface.load(d->url.path());
        if (d->erase)
            ret &= exiv2Iface.removeGPSInfo();
        else
        {
            ret &= exiv2Iface.setGPSInfo(d->gpsData.altitude(), 
                                         d->gpsData.latitude(), 
                                         d->gpsData.longitude());
        }
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

bool GPSListViewItem::isEnabled()    
{
    return d->enabled;
}

bool GPSListViewItem::isDirty()    
{
    return d->dirty;
}

void GPSListViewItem::eraseGPSInfo(bool e)
{
    d->erase = e;
    d->dirty = true;

    if (e)
        setText(6, i18n("Deleted!"));
    else 
        setText(6, "");

    repaint();
}

void GPSListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    if (isEnabled())
    {
        if ( isDirty() && !d->erase && column >= 2  && column <= 4 )
        {
            QColorGroup _cg( cg );
            QColor c = _cg.text();
            _cg.setColor( QColorGroup::Text, Qt::red );
            KListViewItem::paintCell( p, _cg, column, width, alignment );
            _cg.setColor( QColorGroup::Text, c );
        }
        else if ( isDirty() && d->erase && column == 6)
        {
            QColorGroup _cg( cg );
            QColor c = _cg.text();
            _cg.setColor( QColorGroup::Text, Qt::red );
            KListViewItem::paintCell( p, _cg, column, width, alignment );
            _cg.setColor( QColorGroup::Text, c );
        }
        else
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

