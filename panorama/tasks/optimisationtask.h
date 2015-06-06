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

#ifndef OPTIMISATIONTASK_H
#define OPTIMISATIONTASK_H

// Local includes

#include "task.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

class OptimisationTask : public Task
{

private:

    QUrl&                               autoOptimiserPtoUrl;
    const QUrl&                         ptoUrl;
    const bool                          levelHorizon;
    const bool                          buildGPano;
    const QString                       autooptimiserPath;

    QSharedPointer<QProcess>            process;

public:

    OptimisationTask(const QString& workDirPath, const QUrl& input,
                     QUrl& autoOptimiserPtoUrl, bool levelHorizon, bool gPano,
                     const QString& autooptimiserPath);
    ~OptimisationTask();

    void requestAbort();

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;
};

}  // namespace KIPIPanoramaPlugin

#endif /* OPTIMISATIONTASK_H */
