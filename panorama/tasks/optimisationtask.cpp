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
                                   KUrl& autoOptimiserPtoUrl, KUrl& viewCropPtoUrl,
                                   bool levelHorizon, bool buildGPano,
                                   const QString& autooptimiserPath, const QString& panoModifyPath)
    : Task(parent, OPTIMIZE, workDir), autoOptimiserPtoUrl(&autoOptimiserPtoUrl),
      viewCropPtoUrl(&viewCropPtoUrl), ptoUrl(&input),
      levelHorizon(levelHorizon), buildGPano(buildGPano),
      autooptimiserPath(autooptimiserPath), panoModifyPath(panoModifyPath), process(0)
{}

OptimisationTask::OptimisationTask(const KUrl& workDir, const KUrl& input,
                                   KUrl& autoOptimiserPtoUrl, KUrl& viewCropPtoUrl,
                                   bool levelHorizon, bool buildGPano,
                                   const QString& autooptimiserPath, const QString& panoModifyPath)
    : Task(0, OPTIMIZE, workDir), autoOptimiserPtoUrl(&autoOptimiserPtoUrl),
      viewCropPtoUrl(&viewCropPtoUrl), ptoUrl(&input),
      levelHorizon(levelHorizon), buildGPano(buildGPano),
      autooptimiserPath(autooptimiserPath), panoModifyPath(panoModifyPath), process(0)
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
    (*viewCropPtoUrl) = tmpDir;
    viewCropPtoUrl->setFileName(QString("view_crop_pano.pto"));

    KUrl tmpUrl = tmpDir;
    tmpUrl.setFileName(QString("optimized_view_crop.pto"));

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
    args << viewCropPtoUrl->toLocalFile();
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

    delete process;
    process = new KProcess();
    process->clearProgram();
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setOutputChannelMode(KProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    args.clear();
    args << panoModifyPath;
    args << "-c";               // Center the panorama
    args << "-s";               // Straighten the panorama
    args << "--canvas=AUTO";    // Automatic size
    args << "--crop=AUTO";      // Automatic crop
    args << "-o";
    args << autoOptimiserPtoUrl->toLocalFile();
    args << viewCropPtoUrl->toLocalFile();

    process->setProgram(args);

    kDebug() << "pano_modify command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
