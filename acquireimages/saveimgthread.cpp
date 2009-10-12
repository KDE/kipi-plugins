/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-10-11
 * Description : save image thread
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "saveimgthread.h"
#include "saveimgthread.moc"

// Qt includes

#include <QImage>
#include <QDateTime>

// KDE includes

#include <kdebug.h>

// LibKSane includes

#include <libksane/ksane.h>

// Local includes

#include "kpwriteimage.h"
#include "pluginsversion.h"

using namespace KExiv2Iface;
using namespace KDcrawIface;
using namespace KIPIPlugins;

namespace KIPIAcquireImagesPlugin
{

class SaveImgThreadPriv
{
public:

    SaveImgThreadPriv()
    {
    }

    int        width;
    int        height;
    int        bytesPerLine;
    int        frmt;

    QByteArray ksaneData;

    QImage     img;

    QString    make;
    QString    model;
    QString    format;

    KUrl       newUrl;
};

SaveImgThread::SaveImgThread(QObject *parent)
             : QThread(parent), d(new SaveImgThreadPriv)
{
}

SaveImgThread::~SaveImgThread()
{
    // wait for the thread to finish
    wait();

    delete d;
}

void SaveImgThread::setImageData(const QByteArray& ksaneData, int width, int height,
                                 int bytesPerLine, int ksaneFormat)
{
    d->width        = width;
    d->height       = height;
    d->bytesPerLine = bytesPerLine;
    d->frmt         = ksaneFormat;
    d->ksaneData    = ksaneData;
}

void SaveImgThread::setPreviewImage(const QImage& img)
{
    d->img = img;
}

void SaveImgThread::setTargetFile(const KUrl& url, const QString& format)
{
    d->newUrl = url;
    d->format = format;
}

void SaveImgThread::setScannerModel(const QString& make, const QString& model)
{
    d->make  = make;
    d->model = model;
}

void SaveImgThread::run()
{
    // Perform saving ---------------------------------------------------------------

    QImage prev     = d->img.scaled(1280, 1024, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage thumb    = d->img.scaled(160, 120,   Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QByteArray prof = KPWriteImage::getICCProfilFromFile(RawDecodingSettings::SRGB);

    KExiv2 meta;
    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    meta.setImageDimensions(d->img.size());
    if (d->format != QString("JPEG"))
        meta.setImagePreview(prev);
    meta.setExifThumbnail(thumb);
    meta.setExifTagString("Exif.Image.DocumentName", QString("Scanned Image")); // not i18n
    meta.setExifTagString("Exif.Image.Make",  d->make);
    meta.setXmpTagString("Xmp.tiff.Make",     d->make);
    meta.setExifTagString("Exif.Image.Model", d->model);
    meta.setXmpTagString("Xmp.tiff.Model",    d->model);
    meta.setImageDateTime(QDateTime::currentDateTime());
    meta.setImageOrientation(KExiv2::ORIENTATION_NORMAL);
    meta.setImageColorWorkSpace(KExiv2::WORKSPACE_SRGB);

    KPWriteImage wImageIface;
    if (d->frmt != KSaneIface::KSaneWidget::FormatRGB_16_C)
    {
        QByteArray data((const char*)d->img.bits(), d->img.numBytes());
        wImageIface.setImageData(data, d->img.width(), d->img.height(), false, true, prof, meta);
    }
    else
    {
        // 16 bits color depth image.
        wImageIface.setImageData(d->ksaneData, d->width, d->height, true, false, prof, meta);
    }

    bool ret = false;

    if (d->format == QString("JPEG"))
    {
        ret = wImageIface.write2JPEG(d->newUrl.path());
    }
    else if (d->format == QString("PNG"))
    {
        ret = wImageIface.write2PNG(d->newUrl.path());
    }
    else if (d->format == QString("TIFF"))
    {
        ret = wImageIface.write2TIFF(d->newUrl.path());
    }
    else
    {
        ret = d->img.save(d->newUrl.path(), d->format.toAscii().data());
    }

    emit signalComplete(d->newUrl, ret);
}

}  // namespace KIPIAcquireImagesPlugin
