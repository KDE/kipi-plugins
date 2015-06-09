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

#ifndef COMMANDTASK_H
#define COMMANDTASK_H

// Qt includes

#include <QProcess>

// Local includes

#include "task.h"

namespace KIPIPanoramaPlugin
{

class CommandTask : public Task
{
protected:

    QString                             output;

private:

    QSharedPointer<QProcess>            process;
    QString                             commandPath;

public:

    CommandTask(Action action, const QString& workDirPath, const QString& commandPath);
    ~CommandTask();

    void requestAbort();

protected:

    void runProcess(QStringList& args);
    QString getProgram();
    QString getCommandLine();
    QString getProcessError();
};

}  // namespace KIPIPanoramaPlugin

#endif /* COMMANDTASK_H */
