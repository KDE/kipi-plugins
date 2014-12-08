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

#ifndef CREATEPTOTASK_H
#define CREATEPTOTASK_H

// KDE includes

#include <threadweaver/Job.h>

// Local includes

#include "task.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

class CreatePtoTask : public Task
{

private:

    KUrl* const                         ptoUrl;
    const ItemUrlsMap* const            preProcessedMap;
    const PanoramaFileType              fileType;
    const KUrl::List* const             inputFiles;
    const bool                          addGPlusMetadata;
    const QString&                      huginVersion;

public:

    CreatePtoTask(QObject* parent, const KUrl& workDir, KIPIPanoramaPlugin::PanoramaFileType fileType,
                  KUrl& ptoUrl, const KUrl::List& inputFiles, const KIPIPanoramaPlugin::ItemUrlsMap& preProcessedMap,
                  bool addGPlusMetadata, const QString& huginVersion);
    CreatePtoTask(const KUrl& workDir, KIPIPanoramaPlugin::PanoramaFileType fileType,
                  KUrl& ptoUrl, const KUrl::List& inputFiles, const KIPIPanoramaPlugin::ItemUrlsMap& preProcessedMap,
                  bool addGPlusMetadata, const QString& huginVersion);
    ~CreatePtoTask();

protected:

    void run();

private:

    bool computePreview(const KUrl& inUrl);
    bool convertRaw();

};

}  // namespace KIPIPanoramaPlugin

#endif /* CREATEPTOTASK_H */
