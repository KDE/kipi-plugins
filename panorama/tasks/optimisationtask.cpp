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
    : CommandTask(OPTIMIZE, workDirPath, autooptimiserPath),
      autoOptimiserPtoUrl(autoOptimiserPtoUrl),
      ptoUrl(input),
      levelHorizon(levelHorizon),
      buildGPano(gPano)
{}

OptimisationTask::~OptimisationTask()
{}

void OptimisationTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    autoOptimiserPtoUrl = tmpDir.resolved(QUrl::fromLocalFile(QStringLiteral("auto_op_pano.pto")));

    QStringList args;
    args << QStringLiteral("-am");
    if (levelHorizon)
        args << QStringLiteral("-l");
    if (!buildGPano)
       args << QStringLiteral("-s");
    args << QStringLiteral("-o");
    args << autoOptimiserPtoUrl.toLocalFile();
    args << ptoUrl.toLocalFile();

    runProcess(args);

    qCDebug(KIPIPLUGINS_LOG) << "autooptimiser command line: " << getCommandLine();

    qCDebug(KIPIPLUGINS_LOG) << "autooptimiser output:" << endl << output;
}

}  // namespace KIPIPanoramaPlugin
