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

#include "cpcleantask.h"

// Local includes

#include <kipiplugins_debug.h>
#include "ptofile.h"

namespace KIPIPanoramaPlugin
{

CpCleanTask::CpCleanTask(const QString& workDirPath, const QUrl& input,
                         QUrl& cpCleanPtoUrl, const QString& cpCleanPath)
    : Task(CPCLEAN, workDirPath), cpCleanPtoUrl(cpCleanPtoUrl),
      cpFindPtoUrl(input), cpCleanPath(cpCleanPath), process(0)
{}

CpCleanTask::~CpCleanTask()
{}

void CpCleanTask::requestAbort()
{
    process->kill();
}

void CpCleanTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    cpCleanPtoUrl = tmpDir.resolved(QUrl::fromLocalFile(QString::fromUtf8("cp_pano_clean.pto")));

    process.reset(new QProcess());
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << QString::fromUtf8("-o");
    args << cpCleanPtoUrl.toLocalFile();
    args << cpFindPtoUrl.toLocalFile();

    process->setProgram(cpCleanPath);
    process->setArguments(args);

    qCDebug(KIPIPLUGINS_LOG) << "cpclean command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    qCDebug(KIPIPLUGINS_LOG) << "cpclean output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
