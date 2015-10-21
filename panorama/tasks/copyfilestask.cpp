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

#include "copyfilestask.h"

// Qt includes

#include <QFileInfo>
#include <QDateTime>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIPanoramaPlugin
{

CopyFilesTask::CopyFilesTask(const QString& workDirPath, const QUrl& panoUrl, const QUrl& finalPanoUrl,
                             const QUrl& ptoUrl, const ItemUrlsMap& urls, bool sPTO, bool GPlusMetadata)
    : Task(COPY, workDirPath),
      panoUrl(panoUrl),
      finalPanoUrl(finalPanoUrl),
      ptoUrl(ptoUrl),
      urlList(&urls),
      savePTO(sPTO),
      addGPlusMetadata(GPlusMetadata),
      m_iface(0),
      m_meta(0)
{    
    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
        
        if (m_iface)
            m_meta = m_iface->createMetadataProcessor();
    }
}

CopyFilesTask::~CopyFilesTask()
{
}

void CopyFilesTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    QFile     panoFile(panoUrl.toLocalFile());
    QFile     finalPanoFile(finalPanoUrl.toLocalFile());

    QFileInfo fi(finalPanoUrl.toLocalFile());
    QUrl      finalPTOUrl = finalPanoUrl.adjusted(QUrl::RemoveFilename)
                                        .resolved(QUrl::fromLocalFile(fi.completeBaseName() + QStringLiteral(".pto")));

    QFile     ptoFile(ptoUrl.toLocalFile());
    QFile     finalPTOFile(finalPTOUrl.toLocalFile());

    if (!panoFile.exists())
    {
        errString = i18n("Temporary panorama file does not exists.");
        qCDebug(KIPIPLUGINS_LOG) << "Temporary panorama file does not exists: " << panoUrl.toLocalFile();
        successFlag = false;
        return;
    }

    if (finalPanoFile.exists())
    {
        errString = i18n("A panorama file named <filename>%1</filename> already exists.", finalPanoUrl.fileName());
        qCDebug(KIPIPLUGINS_LOG) << "Final panorama file already exists: " << finalPanoUrl.toLocalFile();
        successFlag = false;
        return;
    }

    if (savePTO && !ptoFile.exists())
    {
        errString = i18n("Temporary project file does not exist.");
        qCDebug(KIPIPLUGINS_LOG) << "Temporary project file does not exists: " << ptoUrl.toLocalFile();
        successFlag = false;
        return;
    }

    if (savePTO && finalPTOFile.exists())
    {
        errString = i18n("A project file named <filename>%1</filename> already exists.", finalPTOUrl.fileName());
        qCDebug(KIPIPLUGINS_LOG) << "Final project file already exists: " << finalPTOUrl.toLocalFile();
        successFlag = false;
        return;
    }

    qCDebug(KIPIPLUGINS_LOG) << "Copying GPS info...";

    // Find first src image which contain geolocation and save it to target pano file. 

    double lat, lng, alt;

    for (ItemUrlsMap::const_iterator i = urlList->constBegin(); i != urlList->constEnd(); ++i)
    {
        qCDebug(KIPIPLUGINS_LOG) << i.key();

        if (m_meta)
        {
            m_meta->load(i.key());

            if (m_meta->getGPSInfo(alt, lat, lng))
            {
                qCDebug(KIPIPLUGINS_LOG) << "GPS info found and saved in " << panoUrl;
                m_meta->load(panoUrl);
                m_meta->setGPSInfo(alt, lat, lng);
                m_meta->applyChanges();
                break;
            }
        }
    }

    // Restore usual and common metadata from first shot.

    if (m_meta)
    {
        m_meta->load(urlList->constBegin().key());
        QByteArray iptc = m_meta->getIptc();
        QByteArray xmp  = m_meta->getXmp();
        QString make    = m_meta->getExifTagString(QLatin1String("Exif.Image.Make"));
        QString model   = m_meta->getExifTagString(QLatin1String("Exif.Image.Model"));
        QDateTime dt    = m_meta->getImageDateTime();

        m_meta->load(panoUrl);
        m_meta->setIptc(iptc);
        m_meta->setXmp(xmp);
        m_meta->setXmpTagString(QLatin1String("Xmp.tiff.Make"),  make);
        m_meta->setXmpTagString(QLatin1String("Xmp.tiff.Model"), model);
        m_meta->setImageDateTime(dt);

        QString filesList;

        for (ItemUrlsMap::const_iterator i = urlList->constBegin(); i != urlList->constEnd(); ++i)
            filesList.append(i.key().fileName() + QStringLiteral(" ; "));

        filesList.truncate(filesList.length()-3);

        m_meta->setXmpTagString(QLatin1String("Xmp.kipi.PanoramaInputFiles"), filesList);

        // NOTE : See https://developers.google.com/photo-sphere/metadata/ for details
        if (addGPlusMetadata)
        {
            qCDebug(KIPIPLUGINS_LOG) << "Adding PhotoSphere metadata...";
            m_meta->registerXmpNameSpace(QStringLiteral("http://ns.google.com/photos/1.0/panorama/"), QStringLiteral("GPano"));
            m_meta->setXmpTagString(QLatin1String("Xmp.GPano.UsePanoramaViewer"), QStringLiteral("True"));
            m_meta->setXmpTagString(QLatin1String("Xmp.GPano.StitchingSoftware"), QStringLiteral("Panorama Kipi Plugin with Hugin"));
            m_meta->setXmpTagString(QLatin1String("Xmp.GPano.ProjectionType"),    QStringLiteral("equirectangular"));
        }

        m_meta->applyChanges();
    }
    
    qCDebug(KIPIPLUGINS_LOG) << "Copying panorama file...";

    if (!panoFile.copy(finalPanoUrl.toLocalFile()) || !panoFile.remove())
    {
        errString = i18n("Cannot move panorama from <filename>%1</filename> to <filename>%2</filename>.",
                         panoUrl.toLocalFile(),
                         finalPanoUrl.toLocalFile());
        qCDebug(KIPIPLUGINS_LOG) << "Cannot move panorama: QFile error = " << panoFile.error();
        successFlag = false;
        return;
    }

    if (savePTO)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Copying project file...";

        if (!ptoFile.copy(finalPTOUrl.toLocalFile()))
        {
            errString = i18n("Cannot move project file from <filename>%1</filename> to <filename>%2</filename>.",
                             panoUrl.toLocalFile(),
                             finalPanoUrl.toLocalFile());
            successFlag = false;
            return;
        }

        qCDebug(KIPIPLUGINS_LOG) << "Copying converted RAW files...";

        QPointer<RawProcessor> rawdec = 0;

        if (m_iface)
        {
            rawdec = m_iface->createRawProcessor();
        }
        
        for (ItemUrlsMap::const_iterator i = urlList->constBegin(); i != urlList->constEnd(); ++i)
        {
            if (rawdec && rawdec->isRawFile(i.key()))
            {
                QUrl finalImgUrl = finalPanoUrl.adjusted(QUrl::RemoveFilename)
                                               .resolved(QUrl::fromLocalFile(i->preprocessedUrl.fileName()));
                QFile finalImgFile(finalImgUrl.toString(QUrl::PreferLocalFile));
                QFile imgFile(i->preprocessedUrl.toLocalFile());

                if (finalImgFile.exists())
                {
                    continue;
                }

                if (!imgFile.copy(finalImgUrl.toLocalFile()))
                {
                    errString = i18n("Cannot copy converted image file from <filename>%1</filename> to <filename>%2</filename>.",
                                     i->preprocessedUrl.toLocalFile(),
                                     finalImgUrl.toLocalFile());
                    successFlag = false;
                    return;
                }
            }
        }
    }

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
