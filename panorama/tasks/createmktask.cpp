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
                           KUrl& panoUrl, PanoramaFileType fileType, const QString& pto2mkPath,
                           const QString& nonaPath, const QString& enblendPath, bool preview)
    : Task(parent, preview ? CREATEMKPREVIEW : CREATEMK, workDir), ptoUrl(&input), mkUrl(&mkUrl),
      panoUrl(&panoUrl), fileType(fileType), pto2mkPath(pto2mkPath),
      nonaPath(nonaPath), enblendPath(enblendPath), process(0)
{}

CreateMKTask::CreateMKTask(const KUrl& workDir, const KUrl& input, KUrl& mkUrl,
                           KUrl& panoUrl, PanoramaFileType fileType, const QString& pto2mkPath,
                           const QString& nonaPath, const QString& enblendPath, bool preview)
    : Task(0, preview ? CREATEMKPREVIEW : CREATEMK, workDir), ptoUrl(&input), mkUrl(&mkUrl),
      panoUrl(&panoUrl), fileType(fileType), pto2mkPath(pto2mkPath),
      nonaPath(nonaPath), enblendPath(enblendPath), process(0)
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
        delete process;
        process = 0;
        return;
    }

    delete process;
    process = 0;

    /* Just replacing strings in the generated makefile to reflect the
        * location of the binaries of nona and enblend. This ensures that
        * the make process will be able to execute those binaries without
        * worring that any binary is not in the system path.
        */
    QFile mkUrlFile(mkUrl->toLocalFile());
    mkUrlFile.open(QIODevice::ReadWrite);

    QString fileData = mkUrlFile.readAll();
    fileData.replace("NONA=\"nona\"", QString("NONA=\"%1\"").arg(nonaPath));
    fileData.replace("ENBLEND=\"enblend\"", QString("ENBLEND=\"%1\"").arg(enblendPath));

    mkUrlFile.seek(0L);
    mkUrlFile.write(fileData.toAscii());
    mkUrlFile.close();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
