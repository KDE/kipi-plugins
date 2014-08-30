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

#include "compilemksteptask.h"

// Qt includes

#include <QFileInfo>

// KDE includes

#include <klocale.h>
#include <kdebug.h>

namespace KIPIPanoramaPlugin
{

CompileMKStepTask::CompileMKStepTask(QObject* parent, const KUrl& workDir, int id, const KUrl& mkUrl,
                                     const QString& nonaPath, const QString& enblendPath,
                                     const QString& makePath, bool preview)
    : Task(parent, preview ? NONAFILEPREVIEW : NONAFILE, workDir),
      id(id), mkUrl(&mkUrl), nonaPath(nonaPath),
      enblendPath(enblendPath), makePath(makePath), process(0)
{}

CompileMKStepTask::CompileMKStepTask(const KUrl& workDir, int id, const KUrl& mkUrl,
                                     const QString& nonaPath, const QString& enblendPath,
                                     const QString& makePath, bool preview)
    : Task(0, preview ? NONAFILEPREVIEW : NONAFILE, workDir),
      id(id), mkUrl(&mkUrl), nonaPath(nonaPath),
      enblendPath(enblendPath), makePath(makePath), process(0)
{}

CompileMKStepTask::~CompileMKStepTask()
{
    if (process)
    {
        delete process;
        process = 0;
    }
}

void CompileMKStepTask::requestAbort()
{
    process->kill();
}

void CompileMKStepTask::run()
{
    QFileInfo fi(mkUrl->toLocalFile());

    process = new KProcess();
    process->clearProgram();
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setOutputChannelMode(KProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QString mkFile = fi.completeBaseName() + (id >= 10 ? (id >= 100 ? "0" : "00") : "000") + QString::number(id) + ".tif";
    QStringList args;
    args << makePath;
    args << "-f";
    args << mkUrl->toLocalFile();
    args << QString("ENBLEND='%1'").arg(enblendPath);
    args << QString("NONA='%1'").arg(nonaPath);
    args << mkFile;

    process->setProgram(args);
    kDebug() << "make command line: " << process->program();

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    kDebug() << "make job's output (" << mkFile << "):" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
