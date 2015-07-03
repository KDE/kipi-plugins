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

#ifndef PREPROCESSTASK_H
#define PREPROCESSTASK_H

// Qt includes

#include <QPointer>

// LibKDcraw includes

#include <KDCRAW/KDcraw>

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

    const QUrl                  fileUrl;
    ItemPreprocessedUrls&       preProcessedUrl;
    const RawDecodingSettings   settings;
    QPointer<KDcraw>            rawProcess;

public:

    PreProcessTask(const QString& workDirPath, int id, ItemPreprocessedUrls& targetUrls,
                   const QUrl& sourceUrl, const RawDecodingSettings& rawSettings);
    ~PreProcessTask();

    void requestAbort();

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

private:

    bool computePreview(const QUrl& inUrl);
    bool convertRaw();
};

}  // namespace KIPIPanoramaPlugin

#endif /* PREPROCESSTASK_H */
