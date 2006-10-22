/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-11
 * Description : Metadata file list view item.
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

#include <qpainter.h>
#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kio/previewjob.h>
#include <kiconloader.h>

// Local includes.

#include "exiv2iface.h"
#include "metadataitem.h"
#include "metadataitem.moc"

namespace KIPIMetadataEditPlugin
{

class MetadataItemPriv
{
public:

    MetadataItemPriv()
    {
        enabled   = false;
        dirtyExif = false;
        dirtyIptc = false;
        eraseExif = false;
        eraseIptc = false;
        readOnly  = false;
    }

    bool       enabled;
    bool       dirtyExif;
    bool       dirtyIptc;
    bool       eraseExif;
    bool       eraseIptc;
    bool       readOnly;

    QByteArray exifData;    
    QByteArray iptcData;    

    KURL       url;
};

MetadataItem::MetadataItem(KListView *view, QListViewItem *after, const KURL& url)
            : QObject(view), KListViewItem(view, after)
{
    d = new MetadataItemPriv;
    d->url = url;

    setEnabled(false);
    setPixmap(0, SmallIcon( "file_broken", KIcon::SizeLarge, KIcon::DisabledState ));
    setText(1, d->url.fileName());
    
    // We only add all JPEG files as R/W because Exiv2 can't yet 
    // update metadata on others file formats.

    QFileInfo fi(d->url.path());
    QString ext = fi.extension(false).upper();
    if (ext != QString("JPG") && ext != QString("JPEG") && ext != QString("JPE"))
    {
        setText(4, i18n("Read only"));
        d->readOnly = true;
    }

    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.load(d->url.path());
    setExif(exiv2Iface.getExif(), false);
    setIptc(exiv2Iface.getIptc(), false);

    KIO::PreviewJob* thumbnailJob = KIO::filePreview(url, 64);

    connect(thumbnailJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            this, SLOT(slotGotThumbnail(const KFileItem*, const QPixmap&)));
}

MetadataItem::~MetadataItem()
{
    delete d;
}

void MetadataItem::setExif(const QByteArray& exifData, bool dirty)
{
    setEnabled(true);
    d->dirtyExif = dirty;
    d->exifData  = exifData;
    d->eraseIptc = false;
    setText(2, hasExif() ? i18n("Yes") : i18n("No"));
    repaint();
}

void MetadataItem::setIptc(const QByteArray& iptcData, bool dirty)
{
    setEnabled(true);
    d->dirtyIptc = dirty;
    d->iptcData  = iptcData;
    d->eraseIptc = false;
    setText(3, hasIptc() ? i18n("Yes") : i18n("No"));
    repaint();
}

QByteArray MetadataItem::getExif()
{
    return d->exifData;
}

QByteArray MetadataItem::getIptc()
{
    return d->iptcData;
}

void MetadataItem::eraseExif()
{
    if (!isReadOnly())
    {
        d->eraseExif = true;
        d->dirtyExif = true;
        setText(2, i18n("Removed!"));
        setText(4, i18n("Dirty!"));
        repaint();
    }
}

void MetadataItem::eraseIptc()
{
    if (!isReadOnly())
    {
        d->eraseIptc = true;
        d->dirtyIptc = true;
        setText(3, i18n("Removed!"));
        setText(4, i18n("Dirty!"));
        repaint();
    }
}

KURL MetadataItem::getUrl()
{
    return d->url;
}

bool MetadataItem::hasExif()
{
    return !d->exifData.isEmpty();
}

bool MetadataItem::hasIptc()
{
    return !d->iptcData.isEmpty();
}

void MetadataItem::writeMetadataToFile()
{
    if (isEnabled() && isDirty())
    {
        setPixmap(1, SmallIcon("run"));
        KIPIPlugins::Exiv2Iface exiv2Iface;
        bool ret = exiv2Iface.load(d->url.path());

        if (d->eraseExif)
        {
            ret &= exiv2Iface.clearExif();
        }
        else if (d->dirtyExif)
        {
            ret &= exiv2Iface.setExif(d->exifData);
        }

        if (d->eraseIptc)
        {
            ret &= exiv2Iface.clearIptc();
        }
        else if (d->dirtyIptc)
        {
            ret &= exiv2Iface.setIptc(d->iptcData);
        }

        ret &= exiv2Iface.save(d->url.path());
        
        if (ret)
            setPixmap(1, SmallIcon("ok"));
        else
            setPixmap(1, SmallIcon("cancel"));
	
        d->dirtyExif = false;
        d->dirtyIptc = false;
    }
}

void MetadataItem::setEnabled(bool e)    
{
    d->enabled = e;
    repaint();
}

bool MetadataItem::isEnabled()    
{
    return d->enabled;
}

bool MetadataItem::isDirty()    
{
    return (d->dirtyExif || d->dirtyIptc);
}

bool MetadataItem::isReadOnly()    
{
    return d->readOnly;
}

void MetadataItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    if (isEnabled() && !isReadOnly())
    {
        if ( isDirty() && !d->eraseExif && !d->eraseIptc && column >= 2  && column <= 3 )
        {
            QColorGroup _cg( cg );
            QColor c = _cg.text();
            _cg.setColor( QColorGroup::Text, Qt::red );
            KListViewItem::paintCell( p, _cg, column, width, alignment );
            _cg.setColor( QColorGroup::Text, c );
        }
        else if ( isDirty() && d->eraseExif && d->eraseIptc && column == 4)
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

void MetadataItem::slotGotThumbnail(const KFileItem*, const QPixmap& pix)
{
    setPixmap(0, pix);
    repaint();
}

} // NameSpace KIPIMetadataEditPlugin

