/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2002-12-09
 * Description : a kipi plugin to print images
 *
 * Copyright 2002-2003 by Todd Shoemaker <todd@theshoemakers.net>
 * Copyright 2007-2012 by Angelo Naselli <anaselli at linux dot it>
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

#include "tphoto.h"

// Qt includes

#include <QPainter>
#include <QFileInfo>

// Libkipi includes

#include <KIPI/PluginLoader>

#define IMAGE_FILE_MASK "*"

using namespace KIPI;

namespace KIPIPrintImagesPlugin
{

TPhoto::TPhoto(int thumbnailSize)
    : pAddInfo(0),
      pCaptionInfo(0)
{
    m_size                = 0;
    cropRegion            = QRect(-1, -1, -1, -1);
    rotation              = 0;
    first                 = false;

    copies                = 1;
    //TODO mPrintPosition;
    filename              = QUrl();
    m_meta                = 0;
    m_iface               = 0;

    m_thumbnail           = 0;

    this->m_thumbnailSize = thumbnailSize;
    
    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
    }
}

//to get old photo info
TPhoto::TPhoto (const TPhoto& photo)
    : pAddInfo(0),
      pCaptionInfo(0)
{
    m_thumbnailSize = photo.m_thumbnailSize;
    cropRegion      = photo.cropRegion;
    filename        = photo.filename;
    first           = photo.first;
    copies          = photo.copies;
    rotation        = photo.rotation;

    if (photo.pAddInfo)
    {
        pAddInfo = new AdditionalInfo(*photo.pAddInfo);
    }

    if (photo.pCaptionInfo)
    {
        pCaptionInfo = new CaptionInfo(*photo.pCaptionInfo);
    }

    m_size      = 0;
    m_meta      = 0;
    m_iface     = 0;
    m_thumbnail = 0;
    
    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
    }

}

TPhoto::~TPhoto()
{
    delete m_thumbnail;
    delete m_size;
    delete pAddInfo;
    delete pCaptionInfo;
}

void TPhoto::loadCache()
{
    // load the thumbnail and size only once.
    delete m_thumbnail;

    QImage photo = loadPhoto();
    QImage image = photo.scaled(m_thumbnailSize, m_thumbnailSize, Qt::KeepAspectRatio);
    m_thumbnail  = new QPixmap(image.width(), image.height());
    QPainter painter(m_thumbnail);
    painter.drawImage(0, 0, image );
    painter.end();

    delete m_size;
    m_size = new QSize(photo.width(), photo.height());
}

QPixmap& TPhoto::thumbnail()
{
    if (!m_thumbnail)
        loadCache();

    return *m_thumbnail;
}

QImage TPhoto::loadPhoto()
{
    QImage photo;

    if (m_iface)
    {
        QPointer<RawProcessor> rawdec = m_iface->createRawProcessor();

        // check if its a RAW file.
        if (rawdec && rawdec->isRawFile(filename))
        {
            rawdec->loadRawPreview(filename, photo);
        }
    }
    
    if (photo.isNull())
    {
        photo.load(filename.path());
    }

    return photo;
}

QSize& TPhoto::size()  // private
{
    if (m_size == 0)
        loadCache();

    return *m_size;
}

MetadataProcessor* TPhoto::metaIface()
{
    if (!m_iface)
        return 0;

    if (!m_meta && !filename.url().isEmpty())
    {
        m_meta = m_iface->createMetadataProcessor();
        m_meta->load(filename);
    }

    return m_meta;
}

int TPhoto::width()
{
    return size().width();
}

int TPhoto::height()
{
    return size().height();
}

double TPhoto::scaleWidth(double unitToInches)
{
    Q_ASSERT(pAddInfo != 0);

    cropRegion = QRect(0, 0,
                       (int)(pAddInfo->mPrintWidth * unitToInches),
                       (int)(pAddInfo->mPrintHeight * unitToInches));

    return pAddInfo->mPrintWidth * unitToInches;
}

double TPhoto::scaleHeight(double unitToInches)
{
    Q_ASSERT(pAddInfo != 0);

    cropRegion = QRect(0,
                       0,
                       (int)(pAddInfo->mPrintWidth * unitToInches),
                       (int)(pAddInfo->mPrintHeight * unitToInches));

    return pAddInfo->mPrintHeight * unitToInches;
}

}  // NameSpace KIPIPrintImagesPlugin
