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

#include "cpfindtask.h"

// Local includes

#include <kipiplugins_debug.h>

namespace KIPIPanoramaPlugin
{

CpFindTask::CpFindTask(const QString& workDirPath, const QUrl& input,
                       QUrl& cpFindUrl, bool celeste, const QString& cpFindPath)
    : Task(CPFIND, workDirPath),
      cpFindPtoUrl(cpFindUrl),
      celeste(celeste),
      ptoUrl(input),
      cpFindPath(cpFindPath),
      process(0)
{
}

CpFindTask::~CpFindTask()
{}

void CpFindTask::requestAbort()
{
    process->kill();
}

void CpFindTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    // Run CPFind to get control points and order the images
    cpFindPtoUrl = tmpDir.resolved(QUrl::fromLocalFile(QString::fromUtf8("cp_pano.pto")));

    process.reset(new QProcess());
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    if (celeste)
        args << QString::fromUtf8("--celeste");
    args << QString::fromUtf8("-o");
    args << cpFindPtoUrl.toLocalFile();
    args << ptoUrl.toLocalFile();

    process->setProgram(cpFindPath);
    process->setArguments(args);

    qCDebug(KIPIPLUGINS_LOG) << "cpfind command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitStatus() != QProcess::NormalExit)
    {
        qCDebug(KIPIPLUGINS_LOG) << "cpfind output (failed):" << endl << process->readAll();
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }

    qCDebug(KIPIPLUGINS_LOG) << "cpfind output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
