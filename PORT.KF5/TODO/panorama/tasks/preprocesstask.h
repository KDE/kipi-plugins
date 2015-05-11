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

#ifndef PREPROCESSTASK_H
#define PREPROCESSTASK_H

// Qt includes

#include <QPointer>

// KDE includes

#include <threadweaver/Job.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "task.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

class PreProcessTask : public Task
{

public:

    const int                   id;

private:

    const KUrl                  fileUrl;
    ItemPreprocessedUrls* const preProcessedUrl;
    const RawDecodingSettings   settings;
    QPointer<KDcraw>            rawProcess;

public:

    PreProcessTask(QObject* const parent, const KUrl& workDir, int id, ItemPreprocessedUrls& targetUrls,
                   const KUrl& sourceUrl, const RawDecodingSettings& rawSettings);
    PreProcessTask(const KUrl& workDir, int id, ItemPreprocessedUrls& targetUrls,
                   const KUrl& sourceUrl, const RawDecodingSettings& rawSettings);
    ~PreProcessTask();

    void requestAbort();

protected:

    void run();

private:

    bool computePreview(const KUrl& inUrl);
    bool convertRaw();
};

}  // namespace KIPIPanoramaPlugin

#endif /* PREPROCESSTASK_H */
