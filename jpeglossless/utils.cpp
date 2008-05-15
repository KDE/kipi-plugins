/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : misc utils to used in batch process
 * 
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2004-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C Ansi includes.

extern "C" 
{
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <unistd.h>
}

// Qt includes.

#include <qfileinfo.h>
#include <qimage.h>
#include <qstring.h>
#include <qfile.h>
#include <qdir.h>

// KDE includes.

#include <ktempfile.h>
#include <kprocess.h>
#include <klocale.h>
#include <kurl.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.

#include <libkdcraw/dcrawbinary.h>

// Local includes.

#include "pluginsversion.h"
#include "utils.h"
#include "utils.moc"

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
    QImage iptcPreview   = img.scale(1280, 1024, QImage::ScaleMin);
    QImage exifThumbnail = iptcPreview.scale(160, 120, QImage::ScaleMin);

    KExiv2Iface::KExiv2 meta;
    meta.load(src);
    meta.setImageOrientation(KExiv2Iface::KExiv2::ORIENTATION_NORMAL);
    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    meta.setImageDimensions(img.size());
    meta.setExifThumbnail(exifThumbnail);
    meta.setImagePreview(iptcPreview);
    QByteArray ba = meta.getExif();
    const uchar exifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
    QByteArray exifData = QByteArray(ba.size() + sizeof(exifHeader));
    memcpy(exifData.data(), exifHeader, sizeof(exifHeader));
    memcpy(exifData.data()+sizeof(exifHeader), ba.data(), ba.size());
    QByteArray iptcData = meta.getIptc(true);

    KTempFile exifTemp(QString(), "kipipluginsiptc.app1");
    exifTemp.setAutoDelete(true);
    QFile *exifFile = exifTemp.file();
    if ( !exifFile )
    {
        err = i18n("unable to open temp file");
        return false;
    }
    QDataStream streamExif( exifFile );
    streamExif.writeRawBytes(exifData.data(), exifData.size());
    exifFile->close();

    KTempFile iptcTemp(QString(), "kipipluginsiptc.8bim");
    iptcTemp.setAutoDelete(true);
    QFile *iptcFile = iptcTemp.file();
    if ( !iptcFile )
    {
        err = i18n("Cannot rotate: unable to open temp file");
        return false;
    }
    QDataStream streamIptc( iptcFile );
    streamIptc.writeRawBytes(iptcData.data(), iptcData.size());
    iptcFile->close();

    KProcess process;
    process.clearArguments();
    process << "mogrify";
    process << "-verbose";

    process << "-profile";
    process << exifTemp.name();

    process << "-profile";
    process << iptcTemp.name();

    process << src + QString("[0]");

    qDebug("ImageMagick Command line args:");
    QValueList<QCString> args = process.args();
    for (QValueList<QCString>::iterator it = args.begin(); it != args.end(); ++it)
        qDebug("%s", (const char*)(*it));

    connect(&process, SIGNAL(receivedStderr(KProcess *, char*, int)),
            this, SLOT(slotReadStderr(KProcess*, char*, int)));

    if (!process.start(KProcess::Block, KProcess::Stderr))
        return false;

    if (!process.normalExit())
        return false;

    switch (process.exitStatus())
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
    err = i18n("Cannot update metadata: %1").arg(m_stdErr.replace('\n', ' '));
    return false;
}

void Utils::slotReadStderr(KProcess*, char* buffer, int buflen)
{
    m_stdErr.append(QString::fromLocal8Bit(buffer, buflen));
}

bool Utils::isJPEG(const QString& file)
{
    QString format = QString(QImageIO::imageFormat(file)).upper();
    return format=="JPEG";
}

bool Utils::isRAW(const QString& file)
{
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());

    QFileInfo fileInfo(file);
    if (rawFilesExt.upper().contains( fileInfo.extension(false).upper() ))
        return true;

    return false;
}

bool Utils::CopyFile(const QString& src, const QString& dst)
{
    QFile sFile(src);
    QFile dFile(dst);

    if ( !sFile.open(IO_ReadOnly) )
        return false;

    if ( !dFile.open(IO_WriteOnly) )
    {
        sFile.close();
        return false;
    }

    const int MAX_IPC_SIZE = (1024*32);
    char buffer[MAX_IPC_SIZE];

    Q_LONG len;
    while ((len = sFile.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if (len == -1 || dFile.writeBlock(buffer, (Q_ULONG)len) == -1)
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

bool Utils::MoveFile(const QString& src, const QString& dst)
{
    struct stat stbuf;
    if (::stat(QFile::encodeName(dst), &stbuf) != 0)
    {
        qDebug("KIPIJPEGLossLessPlugin:MoveFile: failed to stat src");
        return false;
    }
    
    if (!CopyFile(src, dst))
        return false;

    struct utimbuf timbuf;
    timbuf.actime = stbuf.st_atime;
    timbuf.modtime = stbuf.st_mtime;
    if (::utime(QFile::encodeName(dst), &timbuf) != 0)
    {
        qDebug("KIPIJPEGLossLessPlugin:MoveFile: failed to update dst time");
    }
    
    if (::unlink(QFile::encodeName(src).data()) != 0)
    {
        qDebug("KIPIJPEGLossLessPlugin:MoveFile: failed to unlink src");
    }
    return true;
}

bool Utils::deleteDir(const QString& dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) 
        return false;

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);

    const QFileInfoList* infoList = dir.entryInfoList();
    if (!infoList) 
        return false;

    QFileInfoListIterator it(*infoList);
    QFileInfo* fi;

    while( (fi = it.current()) ) 
    {
        ++it;
        if(fi->fileName() == "." || fi->fileName() == ".." )
            continue;

        if( fi->isDir() ) 
        {
            deleteDir(fi->absFilePath());
        }
        else if( fi->isFile() )
            dir.remove(fi->absFilePath());
    }

    dir.rmdir(dir.absPath());
    return true;
}

}  // NameSpace KIPIJPEGLossLessPlugin
