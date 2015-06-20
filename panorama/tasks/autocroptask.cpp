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
    : CommandTask(AUTOCROP, workDirPath, panoModifyPath),
      autoOptimiserPtoUrl(autoOptimiserPtoUrl),
      viewCropPtoUrl(viewCropPtoUrl)/*,
      buildGPano(buildGPano),*/
{}

AutoCropTask::~AutoCropTask()
{}

void AutoCropTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    viewCropPtoUrl = tmpDir.resolved(QUrl::fromLocalFile(QStringLiteral("view_crop_pano.pto")));

    QStringList args;
    args << QStringLiteral("-c");               // Center the panorama
    args << QStringLiteral("-s");               // Straighten the panorama
    args << QStringLiteral("--canvas=AUTO");    // Automatic size
    args << QStringLiteral("--crop=AUTO");      // Automatic crop
    args << QStringLiteral("-o");
    args << viewCropPtoUrl.toLocalFile();
    args << autoOptimiserPtoUrl.toLocalFile();

    runProcess(args);

    qCDebug(KIPIPLUGINS_LOG) << "pano_modify command line: " << getCommandLine();

    qCDebug(KIPIPLUGINS_LOG) << "pano_modify output:" << endl << output;
}

}  // namespace KIPIPanoramaPlugin
