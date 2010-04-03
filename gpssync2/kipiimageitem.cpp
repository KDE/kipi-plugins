/* ============================================================
 *
 * Date        : 2010-03-21
 * Description : An item to hold information about an image
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kipiimageitem.h"

// KDE includes

#include <kdebug.h>
#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// local includes

#include "kipiimagemodel.h"

namespace KIPIGPSSyncPlugin
{

KipiImageItem::KipiImageItem(KIPI::Interface* const interface, const KUrl& url, const bool autoLoad)
: m_interface(interface),
  m_url(url),
  m_model(0),
  m_exiv2Iface(0),
  m_exiv2IfaceSaveOnClose(false)
{
    if (autoLoad)
    {
        loadImageData();
    }
}

KipiImageItem::~KipiImageItem()
{
}

void KipiImageItem::openExiv2IFaceIfNotOpen(const bool saveOnClose)
{
    m_exiv2IfaceSaveOnClose|= saveOnClose;

    if (!m_exiv2Iface)
    {
        m_exiv2Iface = new KExiv2Iface::KExiv2;
        m_exiv2Iface->load(m_url.path());
    }
}

void KipiImageItem::closeExiv2IFaceIfOpen()
{
    if (m_exiv2Iface)
    {
        if (m_exiv2IfaceSaveOnClose)
        {
            // TODO: save data
        }

        delete m_exiv2Iface;
        m_exiv2Iface = 0;
        m_exiv2IfaceSaveOnClose = false;
    }
}

void KipiImageItem::loadImageData()
{
    loadImageDataInternal();

    closeExiv2IFaceIfOpen();

    emitDataChanged();
}

void KipiImageItem::loadImageDataInternal()
{
    KIPI::ImageInfo info = m_interface->info(m_url);
    // TODO: this appears to return the file modification date in the default implementation
//     m_dateTime = info.time(KIPI::FromInfo);
//     kDebug()<<m_url<<m_dateTime;
    if (!m_dateTime.isValid())
    {
        openExiv2IFaceIfNotOpen(false);

        m_dateTime = m_exiv2Iface->getImageDateTime();
    }
//     kDebug()<<m_url<<m_dateTime;
}

QVariant KipiImageItem::data(const int column, const int role) const
{
    if ((column==ColumnFilename)&&(role==Qt::DisplayRole))
    {
        return m_url.fileName();
    }
    else if ((column==ColumnDateTime)&&(role==Qt::DisplayRole))
    {
        if (m_dateTime.isValid())
        {
            return m_dateTime.toString(Qt::LocalDate);
        }
        return i18n("Not available");
    }

    return QVariant();
}

bool KipiImageItem::setData(const int column, const int role, const QVariant& value)
{
    return false;
}

void KipiImageItem::setUrl(const KUrl& url)
{
    m_url = url;
    emitDataChanged();
}

void KipiImageItem::setModel(KipiImageModel* const model)
{
    m_model = model;
}

void KipiImageItem::emitDataChanged()
{
    if (m_model)
    {
        m_model->itemChanged(this);
    }
}

void KipiImageItem::setHeaderData(KipiImageModel* const model)
{
    model->setColumnCount(ColumnKipiImageItemCount);
    model->setHeaderData(ColumnThumbnail, Qt::Horizontal, i18n("Thumbnail"), Qt::DisplayRole);
    model->setHeaderData(ColumnFilename, Qt::Horizontal, i18n("Filename"), Qt::DisplayRole);
    model->setHeaderData(ColumnDateTime, Qt::Horizontal, i18n("Date and time"), Qt::DisplayRole);
}

} /* KIPIGPSSyncPlugin */


