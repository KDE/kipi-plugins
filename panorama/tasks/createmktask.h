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

#ifndef CREATEMKTASK_H
#define CREATEMKTASK_H

// KDE includes

#include <threadweaver/Job.h>

// Local includes

#include "task.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

class CreateMKTask : public Task
{

private:

    const KUrl* const                   ptoUrl;
    KUrl* const                         mkUrl;
    KUrl* const                         panoUrl;
    const PanoramaFileType              fileType;
    const QString                       pto2mkPath;

    KProcess*                           process;

public:

    CreateMKTask(QObject* parent, const KUrl& workDir, const KUrl& input, KUrl& mkUrl,
                 KUrl& panoUrl, PanoramaFileType fileType,
                 const QString& pto2mkPath, bool preview);
    CreateMKTask(const KUrl& workDir, const KUrl& input, KUrl& mkUrl,
                 KUrl& panoUrl, PanoramaFileType fileType,
                 const QString& pto2mkPath, bool preview);
    ~CreateMKTask();

    void requestAbort();

protected:

    void run();

};

}  // namespace KIPIPanoramaPlugin

#endif /* CREATEMKTASK_H */
