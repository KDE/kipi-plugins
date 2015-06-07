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

#include "optimisationtask.h"

// Local includes

#include <kipiplugins_debug.h>

namespace KIPIPanoramaPlugin
{

OptimisationTask::OptimisationTask(const QString& workDirPath, const QUrl& input,
                                   QUrl& autoOptimiserPtoUrl, bool levelHorizon, bool gPano,
                                   const QString& autooptimiserPath)
    : Task(OPTIMIZE, workDirPath),
      autoOptimiserPtoUrl(autoOptimiserPtoUrl),
      ptoUrl(input),
      levelHorizon(levelHorizon),
      buildGPano(gPano),
      autooptimiserPath(autooptimiserPath),
      process(0)
{
}

OptimisationTask::~OptimisationTask()
{}

void OptimisationTask::requestAbort()
{
    process->kill();
}

void OptimisationTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    autoOptimiserPtoUrl = tmpDir.resolved(QUrl::fromLocalFile(QString::fromUtf8("auto_op_pano.pto")));

    process.reset(new QProcess());
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << QString::fromUtf8("-am");

    if (levelHorizon)
    {
        args << QString::fromUtf8("-l");
    }

    if (!buildGPano)
    {
       args << QString::fromUtf8("-s");
    }

    args << QString::fromUtf8("-o");
    args << autoOptimiserPtoUrl.toLocalFile();
    args << ptoUrl.toLocalFile();

    process->setProgram(autooptimiserPath);
    process->setArguments(args);

    qCDebug(KIPIPLUGINS_LOG) << "autooptimiser command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        qCDebug(KIPIPLUGINS_LOG) << "autooptimiser output (failed):" << endl << process->readAll();
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    qCDebug(KIPIPLUGINS_LOG) << "autooptimiser output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
