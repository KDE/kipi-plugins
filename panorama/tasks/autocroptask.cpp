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

#include "autocroptask.h"

// Local includes

#include <kipiplugins_debug.h>

namespace KIPIPanoramaPlugin
{

AutoCropTask::AutoCropTask(const QString& workDirPath,
                           const QUrl& autoOptimiserPtoUrl, QUrl& viewCropPtoUrl,
                           bool /*buildGPano*/, const QString& panoModifyPath)
    : Task(AUTOCROP, workDirPath), autoOptimiserPtoUrl(autoOptimiserPtoUrl),
      viewCropPtoUrl(viewCropPtoUrl), /*buildGPano(buildGPano),*/
      panoModifyPath(panoModifyPath), process(0)
{}

AutoCropTask::~AutoCropTask()
{}

void AutoCropTask::requestAbort()
{
    process->kill();
}

void AutoCropTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    viewCropPtoUrl = tmpDir.resolved(QUrl::fromLocalFile(QString::fromUtf8("view_crop_pano.pto")));

    process.reset(new QProcess());
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QStringList args;
    args << QString::fromUtf8("-c");               // Center the panorama
    args << QString::fromUtf8("-s");               // Straighten the panorama
    args << QString::fromUtf8("--canvas=AUTO");    // Automatic size
    args << QString::fromUtf8("--crop=AUTO");      // Automatic crop
    args << QString::fromUtf8("-o");
    args << viewCropPtoUrl.toLocalFile();
    args << autoOptimiserPtoUrl.toLocalFile();

    process->setProgram(panoModifyPath);
    process->setArguments(args);

    qCDebug(KIPIPLUGINS_LOG) << "pano_modify command line: " << process->program() << " " << process->arguments().join(QString::fromUtf8(" "));

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
    qCDebug(KIPIPLUGINS_LOG) << "pano_modify output (failed):" << endl << process->readAll();
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    qCDebug(KIPIPLUGINS_LOG) << "pano_modify output:" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
