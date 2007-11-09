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

#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QtDebug>

// KDE includes.

#include <klocale.h>

// Local includes.

#include "imageresize.h"

namespace KIPISendimagesPlugin
{

ImageResize::ImageResize()
{
}

ImageResize::~ImageResize()
{
}

bool ImageResize::resize(const KUrl& src, const EmailSettingsContainer& settings, QString& err)
{
    QFileInfo fi(src.path());

    if (!fi.exists() || !fi.isReadable()) 
    {
        err = i18n("Error in opening input file");
        return false;
    }

/*
    if ( !m_tmpFile.open() )
    {
        err = i18n("Error in opening temporary file");
        return false;
    }

    QString tmp = m_tmpFile.fileName();

    if (Utils::isRAW(src))
    {
        err = i18n("Cannot rotate RAW file");
        return false;
    }    
    else if (Utils::isJPEG(src))
    {
        if (!rotateJPEG(src, tmp, angle, err))
            return false;
    }
    else
    {
        // B.K.O #123499 : we using Image Magick API here instead QT API 
        // else TIFF/PNG 16 bits image are broken!
        if (!rotateImageMagick(src, tmp, angle, err))
            return false;

        // We update metadata on new image.
        Utils tools(this);
        if (!tools.updateMetadataImageMagick(tmp, err))
            return false;
    }

    // Move back to original file
    if (!Utils::MoveFile(tmp, src)) 
    {
        err = i18n("Cannot update source image");
        return false;
    }
*/
    return true;
}

}  // NameSpace KIPISendimagesPlugin
