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

#include "autocroptask.h"

// KDE includes

#include <klocale.h>
#include <kdebug.h>

namespace KIPIPanoramaPlugin
{

AutoCropTask::AutoCropTask(QObject* parent, const KUrl& workDir,
                           const KUrl& autoOptimiserPtoUrl, KUrl& viewCropPtoUrl,
                           bool buildGPano, const QString& panoModifyPath)
    : Task(parent, AUTOCROP, workDir), autoOptimiserPtoUrl(&autoOptimiserPtoUrl),
      viewCropPtoUrl(&viewCropPtoUrl), buildGPano(buildGPano),
      panoModifyPath(panoModifyPath), process(0)
{}

AutoCropTask::AutoCropTask(const KUrl& workDir,
                           const KUrl& autoOptimiserPtoUrl, KUrl& viewCropPtoUrl,
                           bool buildGPano, const QString& panoModifyPath)
    : Task(0, AUTOCROP, workDir), autoOptimiserPtoUrl(&autoOptimiserPtoUrl),
      viewCropPtoUrl(&viewCropPtoUrl), buildGPano(buildGPano),
      panoModifyPath(panoModifyPath), process(0)
{}

AutoCropTask::~AutoCropTask()
{
    if (process)
    {
        delete process;
        process = 0;
    }
}

void AutoCropTask::requestAbort()
{
    process->kill();
}

void AutoCropTask::run()
{
    (*viewCropPtoUrl) = tmpDir;
    viewCropPtoUrl->setFileName(QString("view_crop_pano.pto"));

    process = new KProcess();
    process->clearProgram();
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setOutputChannelMode(KProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << panoModifyPath;
    args << "-c";               // Center the panorama
    args << "-s";               // Straighten the panorama
    args << "--canvas=AUTO";    // Automatic size
    args << "--crop=AUTO";      // Automatic crop
    args << "-o";
    args << viewCropPtoUrl->toLocalFile();
    args << autoOptimiserPtoUrl->toLocalFile();

    process->setProgram(args);

    kDebug() << "pano_modify command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    kDebug() << "pano_modify's output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
