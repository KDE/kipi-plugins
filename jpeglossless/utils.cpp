/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : misc utils to used in batch process
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

#include "utils.h"
#include "utils.moc"

// C ANSI includes

extern "C"
{
#include <utime.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
}

// Qt includes

#include <QFileInfo>
#include <QImageReader>
#include <QImage>
#include <QFile>
#include <QDir>

// KDE includes

#include <kprocess.h>
#include <kdebug.h>
#include <ktemporaryfile.h>
#include <klocale.h>
#include <kurl.h>

// LibKExiv2 includes 

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// Local includes

#include "pluginsversion.h"

namespace KIPIJPEGLossLessPlugin
{

Utils::Utils(QObject *parent)
     : QObject(parent)
{
}

Utils::~Utils()
{
}

bool Utils::updateMetadataImageMagick(const QString& src, QString& err)
{
    QFileInfo finfo(src);
    if (src.isEmpty() || !finfo.isReadable())
    {
        err = i18n("unable to open source file");
        return false;
    }

    QImage img(src);
    QImage iptcPreview   = img.scaled(1280, 1024, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage exifThumbnail = iptcPreview.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    KExiv2Iface::KExiv2 meta;
    meta.load(src);
    meta.setImageOrientation(KExiv2Iface::KExiv2::ORIENTATION_NORMAL);
    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    meta.setImageDimensions(img.size());
    meta.setExifThumbnail(exifThumbnail);
    meta.setImagePreview(iptcPreview);
    QByteArray exifData = meta.getExif(true);
    QByteArray iptcData = meta.getIptc(true);
    QByteArray xmpData  = meta.getXmp();

    KTemporaryFile exifTemp;
    exifTemp.setSuffix(QString("kipipluginsexif.app1"));
    exifTemp.setAutoRemove(true);
    if ( !exifTemp.open() )
    {
        err = i18n("unable to open temp file");
        return false;
    }
    QString exifFile = exifTemp.fileName();
    QDataStream streamExif( &exifTemp );
    streamExif.writeRawData(exifData.data(), exifData.size());
    exifTemp.close();

    KTemporaryFile iptcTemp;
    iptcTemp.setSuffix(QString("kipipluginsiptc.8bim"));
    iptcTemp.setAutoRemove(true);
    iptcTemp.open();
    if ( !iptcTemp.open() )
    {
        err = i18n("Cannot rotate: unable to open temp file");
        return false;
    }
    QString iptcFile = iptcTemp.fileName();
    QDataStream streamIptc( &iptcTemp );
    streamIptc.writeRawData(iptcData.data(), iptcData.size());
    iptcTemp.close();

    KTemporaryFile xmpTemp;
    xmpTemp.setSuffix(QString("kipipluginsxmp.xmp"));
    xmpTemp.setAutoRemove(true);
    if ( !xmpTemp.open() )
    {
        err = i18n("unable to open temp file");
        return false;
    }
    QString xmpFile = xmpTemp.fileName();
    QDataStream streamXmp( &xmpTemp );
    streamXmp.writeRawData(xmpData.data(), xmpData.size());
    xmpTemp.close();

    KProcess process;
    process.clearProgram();
    process << "mogrify";

    process << "-profile";
    process << exifFile;

    process << "-profile";
    process << iptcFile;

    process << "-profile";
    process << xmpFile;

    process << src + QString("[0]");

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
    err      = i18n("Cannot update metadata: %1", m_stdErr.replace('\n', ' '));
    return false;
}

bool Utils::isJPEG(const QString& file)
{
    QString format = QString(QImageReader::imageFormat(file)).toUpper();
    return format=="JPEG";
}

bool Utils::isRAW(const QString& file)
{
#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    QFileInfo fileInfo(file);
    if (rawFilesExt.toUpper().contains( fileInfo.suffix().toUpper() ))
        return true;

    return false;
}

bool Utils::copyOneFile(const QString& src, const QString& dst)
{
    QFile sFile(src);
    QFile dFile(dst);

    if ( !sFile.open(QIODevice::ReadOnly) )
        return false;

    if ( !dFile.open(QIODevice::WriteOnly) )
    {
        sFile.close();
        return false;
    }

    const int MAX_IPC_SIZE = (1024*32);
    char buffer[MAX_IPC_SIZE];

    qint64 len;
    while ((len = sFile.read(buffer, MAX_IPC_SIZE)) != 0)
    {
        if (len == -1 || dFile.write(buffer, (qint64)len) == -1)
        {
            sFile.close();
            dFile.close();
            return false;
        }
    }

    sFile.close();
    dFile.close();

    return true;
}

bool Utils::moveOneFile(const QString& src, const QString& dst)
{
    struct stat stbuf;
    if (::stat(QFile::encodeName(dst), &stbuf) != 0)
    {
        kError() << "KIPIJPEGLossLessPlugin:moveOneFile: failed to stat src";
        return false;
    }

    if (!copyOneFile(src, dst))
        return false;

    struct utimbuf timbuf;
    timbuf.actime  = stbuf.st_atime;
    timbuf.modtime = stbuf.st_mtime;
    if (::utime(QFile::encodeName(dst), &timbuf) != 0)
    {
        kError() << "KIPIJPEGLossLessPlugin:moveOneFile: failed to update dst time";
    }

    if (::unlink(QFile::encodeName(src).data()) != 0)
    {
        kError() << "KIPIJPEGLossLessPlugin:moveOneFile: failed to unlink src";
    }
    return true;
}

bool Utils::deleteDir(const QString& dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) 
        return false;

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);

    QFileInfoList infoList = dir.entryInfoList();
    if (infoList.isEmpty()) 
        return false;

    QFileInfoList::iterator it = infoList.begin();
    QFileInfo fi;

    while( it != infoList.end() ) 
    {
        fi = *it;
        if(fi.fileName() == "." || fi.fileName() == ".." )
        {
            ++it;
            continue;
        }

        if( fi.isDir() ) 
        {
            deleteDir(fi.absoluteFilePath());
        }
        else if( fi.isFile() )
            dir.remove(fi.absoluteFilePath());

        ++it;
    }

    dir.rmdir(dir.absolutePath());
    return true;
}

}  // namespace KIPIJPEGLossLessPlugin
