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

#include "optimisationtask.h"

// KDE includes

#include <klocale.h>
#include <kdebug.h>

namespace KIPIPanoramaPlugin
{

OptimisationTask::OptimisationTask(QObject* parent, const KUrl& workDir, const KUrl& input,
                                   KUrl& autoOptimiserPtoUrl, bool levelHorizon,
                                   const QString& autooptimiserPath)
    : Task(parent, OPTIMIZE, workDir), autoOptimiserPtoUrl(&autoOptimiserPtoUrl),
      ptoUrl(&input), levelHorizon(levelHorizon),
      autooptimiserPath(autooptimiserPath), process(0)
{}

OptimisationTask::OptimisationTask(const KUrl& workDir, const KUrl& input,
                                   KUrl& autoOptimiserPtoUrl, bool levelHorizon,
                                   const QString& autooptimiserPath)
    : Task(0, OPTIMIZE, workDir), autoOptimiserPtoUrl(&autoOptimiserPtoUrl),
      ptoUrl(&input), levelHorizon(levelHorizon),
      autooptimiserPath(autooptimiserPath), process(0)
{}

OptimisationTask::~OptimisationTask()
{
    if (process)
    {
        delete process;
        process = 0;
    }
}

void OptimisationTask::requestAbort()
{
    process->kill();
}

void OptimisationTask::run()
{
    (*autoOptimiserPtoUrl) = tmpDir;
    autoOptimiserPtoUrl->setFileName(QString("auto_op_pano.pto"));

    process = new KProcess();
    process->clearProgram();
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setOutputChannelMode(KProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << autooptimiserPath;
    args << "-am";
    if (levelHorizon)
    {
        args << "-l";
    }
//     if (!buildGPano)
//     {
        args << "-s";
//     }
    args << "-o";
    args << autoOptimiserPtoUrl->toLocalFile();
    args << ptoUrl->toLocalFile();

    process->setProgram(args);

    kDebug() << "autooptimiser command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    kDebug() << "autooptimiser's output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
