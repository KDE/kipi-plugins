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

#include "compilemksteptask.h"

// Qt includes

#include <QFileInfo>

// Local includes

#include <kipiplugins_debug.h>

namespace KIPIPanoramaPlugin
{

CompileMKStepTask::CompileMKStepTask(const QString& workDirPath, int id, const QUrl& mkUrl,
                                     const QString& nonaPath, const QString& enblendPath,
                                     const QString& makePath, bool preview)
    : Task(preview ? NONAFILEPREVIEW : NONAFILE, workDirPath),
      id(id), mkUrl(mkUrl), nonaPath(nonaPath),
      enblendPath(enblendPath), makePath(makePath), process(0)
{}

CompileMKStepTask::~CompileMKStepTask()
{}

void CompileMKStepTask::requestAbort()
{
    process->kill();
}

void CompileMKStepTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    QFileInfo fi(mkUrl.toLocalFile());

    process.reset(new QProcess());
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    QString mkFile = fi.completeBaseName() + QString::fromUtf8(id >= 10 ? (id >= 100 ? "0" : "00") : "000") + QString::number(id) + QString::fromUtf8(".tif");
    QStringList args;
    args << QString::fromUtf8("-f");
    args << mkUrl.toLocalFile();
    args << QString::fromUtf8("ENBLEND='%1'").arg(enblendPath);
    args << QString::fromUtf8("NONA='%1'").arg(nonaPath);
    args << mkFile;

    process->setProgram(makePath);
    process->setArguments(args);
    qCDebug(KIPIPLUGINS_LOG) << "make job command line: " << process->program() << " " << process->arguments().join(QString::fromUtf8(" "));

    process->start();

    if (!process->waitForFinished(-1) || process->exitCode() != 0)
    {
        errString = getProcessError(*process);
        successFlag = false;
        return;
    }
    qCDebug(KIPIPLUGINS_LOG) << "make job output (" << mkFile << "):" << endl << process->readAll();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
