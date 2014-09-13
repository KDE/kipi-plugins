/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-14
 * Description : batch image flip
 *
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2003-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imageflip.moc"

// C++ includes

#include <cstdio>

/*
 * Define libjpeg_EXPORTS: kde-win emerged jpeg lib uses this define to 
 * decide whether to make dllimport (by default) or dllexport. We need to 
 * export. 
 */

#define libjpeg_EXPORTS

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
#include <QFSFileEngine>

// KDE includes

#include <kprocess.h>
#include <kdebug.h>
#include <klocale.h>

// Local includes

#include "transupp.h"
#include "jpegtransform.h"
#include "utils.h"

namespace KIPIJPEGLossLessPlugin
{

ImageFlip::ImageFlip()
    : QObject()
{
    m_tmpFile.setSuffix("kipiplugin-flip");
    m_tmpFile.setAutoRemove(true);
}

ImageFlip::~ImageFlip()
{
}

bool ImageFlip::flip(const QString& src, FlipAction action, QString& err)
{
    QFileInfo fi(src);

    if (!fi.exists() || !fi.isReadable() || !fi.isWritable()) 
    {
        err = i18n("Error in opening input file");
        return false;
    }

    if ( !m_tmpFile.open() )
    {
        err = i18n("Error in opening temporary file");
        return false;
    }

    QString tmp = m_tmpFile.fileName();

//Workaround to close the file
#ifdef _WIN32
    static_cast<QFSFileEngine*>(m_tmpFile.fileEngine())->rename(tmp);
#endif

    if (KPMetadata::isRawFile(src))
    {
        err = i18n("Cannot rotate RAW file");
        return false;
    }
    else if (Utils::isJPEG(src))
    {
        if (!flipJPEG(src, tmp, action, err))
            return false;
    }
    else
    {
        // bug #123499 : we using Image Magick API here instead QT API 
        // else TIFF/PNG 16 bits image are broken!
        if (!flipImageMagick(src, tmp, action, err))
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

bool ImageFlip::flipJPEG(const QString& src, const QString& dest, FlipAction action, QString& err)
{
    Matrix transform = Matrix::none;

    switch(action)
    {
        case (FlipHorizontal):
        {
            transform = Matrix::flipHorizontal;
            break;
        }
        case (FlipVertical):
        {
            transform = Matrix::flipVertical;
            break;
        }
        default:
        {
            qCritical() << "ImageFlip: Nonstandard flip action";
            err = i18n("Nonstandard flip action");
            return false;
        }
    }

    return transformJPEG(src, dest, transform, err);
}

bool ImageFlip::flipImageMagick(const QString& src, const QString& dest, FlipAction action, QString& err)
{
    KProcess process;
    process.clearProgram();
    process << "convert";

    switch(action)
    {
        case FlipHorizontal:
        {
            process << "-flop";
            break;
        }
        case FlipVertical:
        {
            process << "-flip";
            break;
        }
        default:
        {
            qCritical() << "ImageFlip: Nonstandard flip action";
            err = i18n("Nonstandard flip action");
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
    err      = i18n("Cannot flip: %1", m_stdErr.replace('\n', ' '));
    return false;
}

}  // namespace KIPIJPEGLossLessPlugin
