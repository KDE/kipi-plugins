/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-14
 * Description : batch image rotation
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#define XMD_H

#include "imagerotate.h"
#include "imagerotate.moc"

// C++ includes

#include <cstdio>

// C ANSI includes

extern "C"
{
#include <sys/types.h>
#include <unistd.h>
#include <jpeglib.h>
}

// Qt includes

#include <QFile>
#include <QFileInfo>

// KDE includes

#include <kprocess.h>
#include <kdebug.h>
#include <klocale.h>

// Local includes

#include "utils.h"
#include "transupp.h"
#include "jpegtransform.h"

namespace KIPIJPEGLossLessPlugin
{

ImageRotate::ImageRotate()
           : QObject()
{
    m_tmpFile.setSuffix("kipiplugin-rotate");
    m_tmpFile.setAutoRemove(true);
}

ImageRotate::~ImageRotate()
{
}

bool ImageRotate::rotate(const QString& src, RotateAction angle, QString& err, bool updateFileTimeStamp)
{
    QFileInfo fi(src);

    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) 
    {
        err = i18n("Error in opening input file");
        return false;
    }

    m_tmpFile.setSuffix("kipiplugin-rotate." + fi.suffix());

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
        if (!rotateJPEG(src, tmp, angle, err, updateFileTimeStamp))
        {
            if (err == "nothing to do") { err=QString::null; return true; }
            return false;
        }
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
    if (!Utils::moveOneFile(tmp, src))
    {
        err = i18n("Cannot update source image");
        return false;
    }

    return true;
}

bool ImageRotate::rotateJPEG(const QString& src, const QString& dest, RotateAction angle,
                             QString& err, bool updateFileTimeStamp)
{
    Matrix transform=Matrix::none;

    switch(angle)
    {
        case (Rot90):
        {
            transform = Matrix::rotate90;
            break;
        }
        case (Rot180):
        {
            transform = Matrix::rotate180;
            break;
        }
        case (Rot270):
        {
            transform = Matrix::rotate270;
            break;
        }
        case (Rot0):
        {
            transform = Matrix::none;
            break;
        }
        default:
        {
            kError() << "ImageRotate: Nonstandard rotation angle";
            err = i18n("Nonstandard rotation angle");
            return false;
        }
    }

    return transformJPEG(src, dest, transform, err, updateFileTimeStamp);
}

bool ImageRotate::rotateImageMagick(const QString& src, const QString& dest, 
                                    RotateAction angle, QString& err)
{
    KProcess process;
    process.clearProgram();
    process << "convert";
    process << "-rotate";

    switch(angle)
    {
        case (Rot90):
        {
            process << "90";
            break;
        }
        case (Rot180):
        {
            process << "180";
            break;
        }
        case (Rot270):
        {
            process << "270";
            break;
        }
        case (Rot0):
        {
            break;
        }
        default:
        {
            kError() << "ImageRotate: Nonstandard rotation angle";
            err = i18n("Nonstandard rotation angle");
            return false;
        }
    }

    process << src + QString("[0]") << dest;

    kDebug() << "ImageMagick Command line: " << process.program();

    process.start();

    if (!process.waitForFinished())
        return false;

    if (process.exitStatus() != QProcess::NormalExit)
        return false;

    switch (process.exitCode())
    {
        case 0:  // Process finished successfully !
        {
            return true;
            break;
        }
        case 15: //  process aborted !
        {
            return false;
            break;
        }
    }

    // Processing error !
    m_stdErr = process.readAllStandardError();
    err      = i18n("Cannot rotate: %1", m_stdErr.replace('\n', ' '));
    return false;
}

}  // namespace KIPIJPEGLossLessPlugin
