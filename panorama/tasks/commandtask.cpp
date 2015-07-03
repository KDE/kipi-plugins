/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-06-07
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "commandtask.h"

// KDE includes

#include <KLocalizedString>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIPanoramaPlugin
{

CommandTask::CommandTask(Action action, const QString& workDirPath, const QString& commandPath)
    : Task(action, workDirPath),
      process(0),
      commandPath(commandPath)
{}

CommandTask::~CommandTask()
{}

void CommandTask::requestAbort()
{
    Task::requestAbort();
    if (!process.isNull())
        process->kill();
}

void CommandTask::runProcess(QStringList& args)
{
    process.reset(new QProcess());
    process->setWorkingDirectory(tmpDir.toLocalFile());
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process->setProgram(commandPath);

    process->setArguments(args);

    process->start();

    successFlag = process->waitForFinished(-1) || process->exitStatus() == QProcess::NormalExit;
    output      = QString::fromLocal8Bit(process->readAll());

    if (!successFlag)
        errString = getProcessError();
}

QString CommandTask::getProgram()
{
    if (process.isNull())
        return QString();
    return process->program();
}

QString CommandTask::getCommandLine()
{
    if (process.isNull())
        return QString();
    return (process->program() + QChar::fromLatin1(' ') + process->arguments().join(QChar::fromLatin1(' ')));
}

QString CommandTask::getProcessError()
{
    if (process.isNull())
        return QString();
    return (xi18n("<title>Cannot run <command>%1</command>:</title><para><message>%2</message></para>", getProgram(), output));
}

}  // namespace KIPIPanoramaPlugin
