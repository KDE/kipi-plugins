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

#include "cpcleantask.h"

// KDE includes

#include <klocale.h>
#include <kdebug.h>

// Local includes

#include "ptofile.h"

namespace KIPIPanoramaPlugin
{

CpCleanTask::CpCleanTask(QObject* parent, const KUrl& workDir, const KUrl& input,
                         KUrl& cpCleanPtoUrl, const QString& cpCleanPath)
    : Task(parent, CPCLEAN, workDir), cpCleanPtoUrl(&cpCleanPtoUrl),
      cpFindPtoUrl(&input), cpCleanPath(cpCleanPath), process(0)
{}

CpCleanTask::CpCleanTask(const KUrl& workDir, const KUrl& input,
                         KUrl& cpCleanPtoUrl, const QString& cpCleanPath)
    : Task(0, CPCLEAN, workDir), cpCleanPtoUrl(&cpCleanPtoUrl),
      cpFindPtoUrl(&input), cpCleanPath(cpCleanPath), process(0)
{}

CpCleanTask::~CpCleanTask()
{
    if (process)
    {
        delete process;
        process = 0;
    }
}

void CpCleanTask::requestAbort()
{
    process->kill();
}

void CpCleanTask::run()
{
    (*cpCleanPtoUrl) = tmpDir;
    cpCleanPtoUrl->setFileName(QString("cp_pano_clean.pto"));

    process = new KProcess();
    process->clearProgram();
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setOutputChannelMode(KProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << cpCleanPath;
    args << "-o";
    args << cpCleanPtoUrl->toLocalFile();
    args << cpFindPtoUrl->toLocalFile();

    process->setProgram(args);

    kDebug() << "CPClean command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    kDebug() << "cpclean's output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
