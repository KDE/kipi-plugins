/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-03-15
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include <klocale.h>
#include <kdebug.h>

// Local includes

#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

CopyFilesTask::CopyFilesTask(QObject* parent, const KUrl& workDir, const KUrl& panoUrl, const KUrl& finalPanoUrl,
                             const KUrl& ptoUrl, const ItemUrlsMap& urls, bool savePTO)
    : Task(parent, COPY, workDir), panoUrl(panoUrl), finalPanoUrl(finalPanoUrl),
      ptoUrl(ptoUrl), urlList(&urls), savePTO(savePTO)
{}

CopyFilesTask::CopyFilesTask(const KUrl& workDir, const KUrl& panoUrl, const KUrl& finalPanoUrl,
                             const KUrl& ptoUrl, const ItemUrlsMap& urls, bool savePTO)
    : Task(0, COPY, workDir), panoUrl(panoUrl), finalPanoUrl(finalPanoUrl),
      ptoUrl(ptoUrl), urlList(&urls), savePTO(savePTO)
{}

CopyFilesTask::~CopyFilesTask()
{}

void CopyFilesTask::run()
{
    QFile panoFile(panoUrl.toLocalFile());
    QFile finalPanoFile(finalPanoUrl.toLocalFile());

    QFileInfo fi(finalPanoUrl.toLocalFile());
    KUrl finalPTOUrl(finalPanoUrl);
    finalPTOUrl.setFileName(fi.completeBaseName() + ".pto");
    QFile ptoFile(ptoUrl.toLocalFile());
    QFile finalPTOFile(finalPTOUrl.toLocalFile());

    if (!panoFile.exists())
    {
        errString = i18n("Temporary panorama file does not exists.");
        kDebug() << "Temporary panorama file does not exists: " + panoUrl.toLocalFile();
        successFlag = false;
        return;
    }
    if (finalPanoFile.exists())
    {
        errString = i18n("A file named %1 already exists.", finalPanoUrl.fileName());
        kDebug() << "Final panorama file already exists: " + finalPanoUrl.toLocalFile();
        successFlag = false;
        return;
    }
    if (savePTO && !ptoFile.exists())
    {
        errString = i18n("Temporary project file does not exist.");
        kDebug() << "Temporary project file does not exists: " + ptoUrl.toLocalFile();
        successFlag = false;
        return;
    }
    if (savePTO && finalPTOFile.exists())
    {
        errString = i18n("A file named %1 already exists.", finalPTOUrl.fileName());
        kDebug() << "Final project file already exists: " + finalPTOUrl.toLocalFile();
        successFlag = false;
        return;
    }

    kDebug() << "Copying panorama file...";
    if (!panoFile.copy(finalPanoUrl.toLocalFile()) || !panoFile.remove())
    {
        errString = i18n("Cannot move panorama from %1 to %2.",
                         panoUrl.toLocalFile(),
                         finalPanoUrl.toLocalFile());
        kDebug() << "Cannot move panorama: QFile error = " + panoFile.error();
        successFlag = false;
        return;
    }

    if (savePTO)
    {
        kDebug() << "Copying project file...";
        if (!ptoFile.copy(finalPTOUrl.toLocalFile()))
        {
            errString = i18n("Cannot move project file from %1 to %2.",
                             panoUrl.toLocalFile(),
                             finalPanoUrl.toLocalFile());
            successFlag = false;
            return;
        }

        kDebug() << "Copying converted RAW files...";
        for (ItemUrlsMap::iterator i = (ItemUrlsMap::iterator) urlList->begin(); i != urlList->end(); ++i)
        {
            if (KPMetadata::isRawFile(i.key()))
            {
                KUrl finalImgUrl(finalPanoUrl);
                finalImgUrl.setFileName(i->preprocessedUrl.fileName());
                QFile imgFile(i->preprocessedUrl.toLocalFile());
                if (!imgFile.copy(finalImgUrl.toLocalFile()))
                {
                    errString = i18n("Cannot copy converted image file from %1 to %2.",
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
