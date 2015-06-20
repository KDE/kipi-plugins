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

#include "createmktask.h"

// Qt includes

#include <QFileInfo>

// Local includes

#include <kipiplugins_debug.h>

namespace KIPIPanoramaPlugin
{

CreateMKTask::CreateMKTask(const QString& workDirPath, const QUrl& input, QUrl& mkUrl,
                           QUrl& panoUrl, PanoramaFileType fileType,
                           const QString& pto2mkPath, bool preview)
    : CommandTask(preview ? CREATEMKPREVIEW : CREATEMK, workDirPath, pto2mkPath),
      ptoUrl(input),
      mkUrl(mkUrl),
      panoUrl(panoUrl),
      fileType(fileType)
{}

CreateMKTask::~CreateMKTask()
{}

void CreateMKTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    QFileInfo fi(ptoUrl.toLocalFile());
    mkUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QLatin1String(".mk")));

    switch (fileType)
    {
        case JPEG:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QLatin1String(".jpg")));
            break;
        case TIFF:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QLatin1String(".tif")));
            break;
        case HDR:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QLatin1String(".hdr")));
            break;
    }

    QStringList args;
    args << QStringLiteral("-o");
    args << mkUrl.toLocalFile();
    args << QStringLiteral("-p");
    args << fi.completeBaseName();
    args << ptoUrl.toLocalFile();

    runProcess(args);

    qCDebug(KIPIPLUGINS_LOG) << "pto2mk command line: " << getCommandLine();

    qCDebug(KIPIPLUGINS_LOG) << "pto2mk output:" << endl << output;
}

}  // namespace KIPIPanoramaPlugin
