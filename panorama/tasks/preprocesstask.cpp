/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-03-15
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "preprocesstask.h"

// Qt includes

#include <QFileInfo>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIPanoramaPlugin
{

PreProcessTask::PreProcessTask(const QString& workDirPath, int id, ItemPreprocessedUrls& targetUrls,
                               const QUrl& sourceUrl)
    : Task(PREPROCESS_INPUT,
      workDirPath),
      id(id),
      fileUrl(sourceUrl),
      preProcessedUrl(targetUrls),
      m_iface(0),     
      m_meta(0),
      m_rawdec(0)
{
    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
                
        if (m_iface)
        {
            m_meta   = m_iface->createMetadataProcessor();
            m_rawdec = m_iface->createRawProcessor();
        }
    }
}

PreProcessTask::~PreProcessTask()
{
}

void PreProcessTask::requestAbort()
{
    Task::requestAbort();

    if (!m_rawdec.isNull())
    {
        m_rawdec->cancel();
    }
}

void PreProcessTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    // check if its a RAW file.
    if (m_rawdec && m_rawdec->isRawFile(fileUrl))
    {
        preProcessedUrl.preprocessedUrl = tmpDir;

        if (!convertRaw())
        {
            successFlag = false;
            return;
        }
    }
    else
    {
        // NOTE: in this case, preprocessed Url is the original file Url.
        preProcessedUrl.preprocessedUrl = fileUrl;
    }

    preProcessedUrl.previewUrl = tmpDir;

    if (!computePreview(preProcessedUrl.preprocessedUrl))
    {
        successFlag = false;
        return;
    }

    successFlag = true;
    return;
}

bool PreProcessTask::computePreview(const QUrl& inUrl)
{
    QUrl& outUrl = preProcessedUrl.previewUrl;

    QFileInfo fi(inUrl.toLocalFile());
    outUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName().replace(QLatin1String("."), QLatin1String("_")) + QStringLiteral("-preview.jpg")));

    QImage img;

    if (img.load(inUrl.toLocalFile()))
    {
        QImage preview = img.scaled(1280, 1024, Qt::KeepAspectRatio);
        bool saved     = preview.save(outUrl.toLocalFile(), "JPEG");

        // save exif information also to preview image for auto rotation
        if (saved && m_meta)
        {
            m_meta->load(inUrl);
            int orientation = m_meta->getImageOrientation();
            
            m_meta->load(outUrl);
            m_meta->setImageOrientation(orientation);
            m_meta->setImageDimensions(QSize(preview.width(), preview.height()));
            m_meta->applyChanges();
        }

        qCDebug(KIPIPLUGINS_LOG) << "Preview Image url: " << outUrl << ", saved: " << saved;
        return saved;
    }
    else
    {
        errString = i18n("Input image cannot be loaded for preview generation.");
    }

    return false;
}

bool PreProcessTask::convertRaw()
{
    if (!m_meta)
    {
        return false;
    }

    const QUrl& inUrl = fileUrl;
    QUrl& outUrl      = preProcessedUrl.preprocessedUrl;
    bool decoded      = false;

    int        width, height, rgbmax;
    QByteArray imageData;

    if (m_rawdec)
    {
        decoded = m_rawdec->decodeRawImage(inUrl, imageData, width, height, rgbmax);
    }            

    if (decoded)
    {
        uchar* sptr  = (uchar*)imageData.data();
        float factor = 65535.0 / rgbmax;
        unsigned short tmp16[3];

        // Set RGB color components.
        for (int i = 0 ; !isAbortedFlag && (i < width * height) ; ++i)
        {
            // Swap Red and Blue and re-ajust color component values
            tmp16[0] = (unsigned short)((sptr[5]*256 + sptr[4]) * factor);      // Blue
            tmp16[1] = (unsigned short)((sptr[3]*256 + sptr[2]) * factor);      // Green
            tmp16[2] = (unsigned short)((sptr[1]*256 + sptr[0]) * factor);      // Red
            memcpy(&sptr[0], &tmp16[0], 6);
            sptr += 6;
        }

        if (isAbortedFlag)
        {
            errString = i18n("Operation canceled.");
            return false;
        }

        m_meta->load(inUrl);
        m_meta->removeExifTags(QStringList() << QStringLiteral("Photo"));
        QByteArray exif = m_meta->getExif();
        QByteArray iptc = m_meta->getIptc();
        QByteArray xmp  = m_meta->getXmp();
        QString make    = m_meta->getExifTagString(QLatin1String("Exif.Image.Make"));
        QString model   = m_meta->getExifTagString(QLatin1String("Exif.Image.Model"));

        QFileInfo fi(inUrl.toLocalFile());
        outUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName().replace(QLatin1String("."), QLatin1String("_")) + QStringLiteral(".tif")));

        // wImageIface.setCancel(&isAbortedFlag);
        
        if (m_iface && !m_iface->saveImage(outUrl, QLatin1String("TIF"),
                                           imageData, width, height,
                                           true, false,
                                           &isAbortedFlag))
        {
            errString = i18n("Tiff image creation failed.");
            return false;
        }

        m_meta->load(outUrl);
        m_meta->setExif(exif);
        m_meta->setIptc(iptc);
        m_meta->setXmp(xmp);
        m_meta->setImageDimensions(QSize(width, height));
        m_meta->setExifTagString(QLatin1String("Exif.Image.DocumentName"), inUrl.fileName());
        m_meta->setXmpTagString(QLatin1String("Xmp.tiff.Make"),  make);
        m_meta->setXmpTagString(QLatin1String("Xmp.tiff.Model"), model);
        m_meta->setImageOrientation(MetadataProcessor::NORMAL);
        m_meta->applyChanges();
    }
    else
    {
        errString = i18n("Raw file conversion failed.");
        return false;
    }

    qCDebug(KIPIPLUGINS_LOG) << "Convert RAW output url: " << outUrl;

    return true;
}

}  // namespace KIPIPanoramaPlugin
