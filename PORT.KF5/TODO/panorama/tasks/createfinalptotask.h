/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-12-05
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

#ifndef CREATEPFINALPTOTASK_H
#define CREATEPFINALPTOTASK_H

// KDE includes

#include <threadweaver/Job.h>

// Local includes

#include "task.h"
#include "ptotype/ptotype.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

class CreateFinalPtoTask : public Task
{

private:

    PTOType                 ptoData;
    KUrl* const             finalPtoUrl;
    const QRect             crop;

public:

    CreateFinalPtoTask(QObject* parent, const KUrl& workDir, const PTOType& ptoData, KUrl& finalPtoUrl, const QRect& crop);
    CreateFinalPtoTask(const KUrl& workDir, const PTOType& ptoData, KUrl& finalPtoUrl, const QRect& crop);
    ~CreateFinalPtoTask();

protected:

    void run();

};

}  // namespace KIPIPanoramaPlugin

#endif /* CREATEPFINALPTOTASK_H */
