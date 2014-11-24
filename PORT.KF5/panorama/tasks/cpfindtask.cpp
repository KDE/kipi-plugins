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

#include "cpfindtask.h"

// KDE includes

#include <klocale.h>
#include <kdebug.h>

namespace KIPIPanoramaPlugin
{

CpFindTask::CpFindTask(QObject* parent, const KUrl& workDir, const KUrl& input,
                       KUrl& cpFindUrl, bool celeste, const QString& cpFindPath)
    : Task(parent, CPFIND, workDir), cpFindPtoUrl(&cpFindUrl),
      celeste(celeste), ptoUrl(&input), cpFindPath(cpFindPath), process(0)
{}

CpFindTask::CpFindTask(const KUrl& workDir, const KUrl& input,
                       KUrl& cpFindUrl, bool celeste, const QString& cpFindPath)
    : Task(0, CPFIND, workDir), cpFindPtoUrl(&cpFindUrl),
      celeste(celeste), ptoUrl(&input), cpFindPath(cpFindPath), process(0)
{}

CpFindTask::~CpFindTask()
{
    if (process)
    {
        delete process;
        process = 0;
    }
}

void CpFindTask::requestAbort()
{
    process->kill();
}

void CpFindTask::run()
{
    // Run CPFind to get control points and order the images
    (*cpFindPtoUrl) = tmpDir;
    cpFindPtoUrl->setFileName(QString("cp_pano.pto"));

    process = new KProcess();
    process->clearProgram();
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setOutputChannelMode(KProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << cpFindPath;
    if (celeste)
        args << "--celeste";
    args << "-o";
    args << cpFindPtoUrl->toLocalFile();
    args << ptoUrl->toLocalFile();

    process->setProgram(args);

    kDebug() << "CPFind command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitStatus() != QProcess::NormalExit)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    kDebug() << "cpfind's output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
