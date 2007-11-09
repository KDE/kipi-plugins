/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-09
 * Description : batch image resize
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

// Qt includes.

#include <QImageReader>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QtDebug>

// KDE includes.

#include <klocale.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/kdcraw.h>

// Local includes.

#include "pluginsversion.h"
#include "imageresize.h"

namespace KIPISendimagesPlugin
{

ImageResize::ImageResize(const EmailSettingsContainer& settings)
{
    m_settings = settings;
}

ImageResize::~ImageResize()
{
}

bool ImageResize::resize(const KUrl& src, const QString& destName, QString& err)
{
    QFileInfo fi(src.path());

    if (!fi.exists() || !fi.isReadable()) 
    {
        err = i18n("Error in opening input file");
        return false;
    }

    QFileInfo tmp(m_settings.tempPath);

    if (!tmp.exists() || !tmp.isWritable())
    {
        err = i18n("Error in opening temporary folder");
        return false;
    }

    QImage img;

    // Check if RAW file.
    QString rawFilesExt(raw_file_extentions);
    if (rawFilesExt.toUpper().contains( fi.suffix().toUpper() ))
        KDcrawIface::KDcraw::loadDcrawPreview(img, src.path());
    else
        img.load(src.path());

    int sizeFactor = m_settings.size();

    if ( !img.isNull() )
    {
        int w = img.width();
        int h = img.height();

        if( w > sizeFactor || h > sizeFactor )
        {
            if( w > h )
            {
                h = (int)( (double)( h * sizeFactor ) / w );
    
                if ( h == 0 ) h = 1;
    
                w = sizeFactor;
                Q_ASSERT( h <= sizeFactor );
            }
            else
            {
                w = (int)( (double)( w * sizeFactor ) / h );
    
                if ( w == 0 ) w = 1;
    
                h = sizeFactor;
                Q_ASSERT( w <= sizeFactor );
            }
    
            const QImage scaledImg(img.scaled(w, h));
    
            if ( scaledImg.width() != w || scaledImg.height() != h )
            {
                err = i18n("Cannot resizing image. Aborting.");
                return false;
            }
    
            img = scaledImg;
        }
    
        QString destPath = m_settings.tempPath + destName;
        qDebug() << destPath << endl;

        if ( !img.save(destPath, m_settings.format().toLatin1(), m_settings.imageCompression) )
        {
            err = i18n("Cannot save resized image. Aborting.");
            return false;
        }
    
        // Only try to write Exif if both src and destination are JPEG files.
    
        if (QString(QImageReader::imageFormat(destPath)).toUpper() == "JPEG" && 
            m_settings.format().toUpper() == "JPEG")
        {
            KExiv2Iface::KExiv2 meta;
    
            if (meta.load(destPath))
            {
                meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
                meta.setImageDimensions(img.size());
                meta.save(destPath);
            }
            else
            {
                err = i18n("Cannot update metadata to resized image. Aborting.");
                return false;
            }
        }

        return true;
    }

    return false;
}

}  // NameSpace KIPISendimagesPlugin
