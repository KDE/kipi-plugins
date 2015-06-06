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
    : Task(preview ? CREATEMKPREVIEW : CREATEMK, workDirPath),
      ptoUrl(input), mkUrl(mkUrl),
      panoUrl(panoUrl),
      fileType(fileType),
      pto2mkPath(pto2mkPath),
      process(0)
{
}

CreateMKTask::~CreateMKTask()
{}

void CreateMKTask::requestAbort()
{
    process->kill();
}

void CreateMKTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    QFileInfo fi(ptoUrl.toLocalFile());
    mkUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QString::fromUtf8(".mk")));

    switch (fileType)
    {
        case JPEG:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QString::fromUtf8(".jpg")));
            break;
        case TIFF:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QString::fromUtf8(".tif")));
            break;
        case HDR:
            panoUrl = tmpDir.resolved(QUrl::fromLocalFile(fi.completeBaseName() + QString::fromUtf8(".hdr")));
            break;
    }

    process.reset(new QProcess());
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << QString::fromUtf8("-o");
    args << mkUrl.toLocalFile();
    args << QString::fromUtf8("-p");
    args << fi.completeBaseName();
    args << ptoUrl.toLocalFile();

    process->setProgram(pto2mkPath);
    process->setArguments(args);

    qCDebug(KIPIPLUGINS_LOG) << "pto2mk command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }

    qCDebug(KIPIPLUGINS_LOG) << "pto2mk output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
