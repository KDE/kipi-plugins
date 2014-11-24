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

#include "createmktask.h"

// Qt includes

#include <QFileInfo>

// KDE includes

#include <klocale.h>
#include <kdebug.h>

namespace KIPIPanoramaPlugin
{

CreateMKTask::CreateMKTask(QObject* parent, const KUrl& workDir, const KUrl& input, KUrl& mkUrl,
                           KUrl& panoUrl, PanoramaFileType fileType,
                           const QString& pto2mkPath, bool preview)
    : Task(parent, preview ? CREATEMKPREVIEW : CREATEMK, workDir), ptoUrl(&input), mkUrl(&mkUrl),
      panoUrl(&panoUrl), fileType(fileType),
      pto2mkPath(pto2mkPath), process(0)
{}

CreateMKTask::CreateMKTask(const KUrl& workDir, const KUrl& input, KUrl& mkUrl,
                           KUrl& panoUrl, PanoramaFileType fileType,
                           const QString& pto2mkPath, bool preview)
    : Task(0, preview ? CREATEMKPREVIEW : CREATEMK, workDir), ptoUrl(&input), mkUrl(&mkUrl),
      panoUrl(&panoUrl), fileType(fileType),
      pto2mkPath(pto2mkPath), process(0)
{}

CreateMKTask::~CreateMKTask()
{
    if (process)
    {
        delete process;
        process = 0;
    }
}

void CreateMKTask::requestAbort()
{
    process->kill();
}

void CreateMKTask::run()
{
    QFileInfo fi(ptoUrl->toLocalFile());
    (*mkUrl) = tmpDir;
    mkUrl->setFileName(fi.completeBaseName() + QString(".mk"));

    (*panoUrl) = tmpDir;
    switch (fileType)
    {
        case JPEG:
            panoUrl->setFileName(fi.completeBaseName() + QString(".jpg"));
            break;
        case TIFF:
            panoUrl->setFileName(fi.completeBaseName() + QString(".tif"));
            break;
        case HDR:
            panoUrl->setFileName(fi.completeBaseName() + QString(".hdr"));
            break;
    }

    process = new KProcess();
    process->clearProgram();
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setOutputChannelMode(KProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << pto2mkPath;
    args << "-o";
    args << mkUrl->toLocalFile();
    args << "-p";
    args << fi.completeBaseName();
    args << ptoUrl->toLocalFile();

    process->setProgram(args);

    kDebug() << "pto2mk command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    kDebug() << "pto2mk's output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
