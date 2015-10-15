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

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"
#include "kpmetadata.h"

using namespace KIPI;
using namespace KIPIPlugins;

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
      addGPlusMetadata(GPlusMetadata)
{
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

        KPMetadata metaSrc(i.key().toLocalFile());

        if(metaSrc.getGPSInfo(alt, lat, lng))
        {
            qCDebug(KIPIPLUGINS_LOG) << "GPS info found and saved in " << panoUrl;
            KPMetadata metaDst(panoUrl.toLocalFile());
            metaDst.setGPSInfo(alt, lat, lng);
            metaDst.applyChanges();
            break;
        }
    }

    // Restore usual and common metadata from first shot.

    KPMetadata metaSrc(urlList->constBegin().key().toLocalFile());
    KPMetadata metaDst(panoUrl.toLocalFile());
    metaDst.setIptc(metaSrc.getIptc());
    metaDst.setXmp(metaSrc.getXmp());
    metaDst.setXmpTagString("Xmp.tiff.Make",  metaSrc.getExifTagString("Exif.Image.Make"));
    metaDst.setXmpTagString("Xmp.tiff.Model", metaSrc.getExifTagString("Exif.Image.Model"));
    metaDst.setImageDateTime(metaSrc.getImageDateTime(), true);

    QString filesList;

    for (ItemUrlsMap::const_iterator i = urlList->constBegin(); i != urlList->constEnd(); ++i)
        filesList.append(i.key().fileName() + QStringLiteral(" ; "));

    filesList.truncate(filesList.length()-3);

    metaDst.setXmpTagString("Xmp.kipi.PanoramaInputFiles", filesList, false);
    metaDst.setImageDateTime(QDateTime::currentDateTime());

    // NOTE : See https://developers.google.com/photo-sphere/metadata/ for details
    if (addGPlusMetadata)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Adding PhotoSphere metadata...";
        metaDst.registerXmpNameSpace(QStringLiteral("http://ns.google.com/photos/1.0/panorama/"), QStringLiteral("GPano"));
        metaDst.setXmpTagString("Xmp.GPano.UsePanoramaViewer", QStringLiteral("True"));
        metaDst.setXmpTagString("Xmp.GPano.StitchingSoftware", QStringLiteral("Panorama Kipi Plugin with Hugin"));
        metaDst.setXmpTagString("Xmp.GPano.ProjectionType",    QStringLiteral("equirectangular"));
    }

    metaDst.applyChanges();

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

        RawProcessor* rawdec   = 0;
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            Interface* const iface = pl->interface();
            
            if (iface)
            {
                rawdec = iface->createRawProcessor();
            }
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
        
        delete rawdec;
    }

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
