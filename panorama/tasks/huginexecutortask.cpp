/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-11-04
 * Description : interface to hugin_executor
 *
 * Copyright (C) 2015-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "huginexecutortask.h"

// Qt includes

#include <QFile>
#include <QFileInfo>

namespace KIPIPanoramaPlugin
{

HuginExecutorTask::HuginExecutorTask(const QString& workDirPath, const QUrl& input,
                                     QUrl& panoUrl, PanoramaFileType fileType,
                                     const QString& huginExecutorPath, bool preview)
    : CommandTask(preview ? HUGINEXECUTORPREVIEW : HUGINEXECUTOR, workDirPath, huginExecutorPath),
      ptoUrl(input),
      panoUrl(panoUrl),
      fileType(fileType)
{}

HuginExecutorTask::~HuginExecutorTask()
{}

void HuginExecutorTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    QFileInfo fi(ptoUrl.toLocalFile());

    switch (fileType)
    {
        case JPEG:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QStringLiteral(".jpg")));
            break;
        case TIFF:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QStringLiteral(".tif")));
            break;
        case HDR:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QStringLiteral(".hdr")));
            break;
    }

    QStringList args;
    args << QStringLiteral("-s");
    args << QStringLiteral("-p");
    args << fi.completeBaseName();
    args << ptoUrl.toLocalFile();

    runProcess(args);

    // HuginExecutor does not return an error code when something went wrong...
    QFile panoOutput(panoUrl.toLocalFile());
    if (!panoOutput.exists())
    {
        successFlag = false;
        errString = getProcessError();
    }

    printDebug(QStringLiteral("hugin_executor"));
}

}  // namespace KIPIPanoramaPlugin
