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

#ifndef AUTOCROPTASK_H
#define AUTOCROPTASK_H

// Local includes

#include "task.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

class AutoCropTask : public Task
{

private:

    const QUrl&                         autoOptimiserPtoUrl;
    QUrl&                               viewCropPtoUrl;
//    const bool                          buildGPano;
    const QString                       panoModifyPath;

    QSharedPointer<QProcess>            process;

public:

    AutoCropTask(const QString& workDirPath,
                 const QUrl& autoOptimiserPtoUrl, QUrl& viewCropPtoUrl,
                 bool buildGPano, const QString& panoModifyPath);
    ~AutoCropTask();

    void requestAbort();

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

};

}  // namespace KIPIPanoramaPlugin

#endif /* AUTOCROPTASK_H */
