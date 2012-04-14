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

#include "task.h"

// Qt includes

#include <QFileInfo>

// KDE includes

#include <klocale.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

namespace KIPIPanoramaPlugin
{

Task::Task(QObject* parent, Action action, const KUrl& workDir)
    : Job(parent), action(action), successFlag(false), isAbortedFlag(false), tmpDir(workDir)
{}

Task::~Task()
{}

bool Task::success() const
{
    return successFlag;
}

void Task::requestAbort()
{
    isAbortedFlag = true;
}

bool Task::isRawFile(const KUrl& url)
{
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());

    QFileInfo fileInfo(url.toLocalFile());
    if (rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper()))
        return true;

    return false;
}

QString Task::getProcessError(KProcess& proc)
{
    QString std = proc.readAll();
    return (i18n("Cannot run %1:\n\n %2", proc.program()[0], std));
}

}  // namespace KIPIPanoramaPlugin
