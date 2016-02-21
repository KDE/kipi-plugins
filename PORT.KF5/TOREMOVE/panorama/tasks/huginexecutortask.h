/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-11-04
 * Description : interface to hugin_executor
 *
 * Copyright (C) 2015-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef HUGINEXECUTORTASK_H
#define HUGINEXECUTORTASK_H

// Local includes

#include "commandtask.h"



namespace KIPIPanoramaPlugin
{

class HuginExecutorTask : public CommandTask
{

private:

    const QUrl&                         ptoUrl;
    QUrl&                               panoUrl;
    const PanoramaFileType              fileType;

public:

    HuginExecutorTask(const QString& workDirPath, const QUrl& input,
                      QUrl& panoUrl, PanoramaFileType fileType,
                      const QString& huginExecutorPath, bool preview);
    ~HuginExecutorTask();

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

};

}  // namespace KIPIPanoramaPlugin

#endif /* HUGINEXECUTORTASK_H */
