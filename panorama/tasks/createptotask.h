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

#ifndef CREATEPTOTASK_H
#define CREATEPTOTASK_H

// Local includes

#include "task.h"



namespace KIPIPanoramaPlugin
{

class CreatePtoTask : public Task
{

private:

    QUrl&                               ptoUrl;
    const ItemUrlsMap* const            preProcessedMap;
    const PanoramaFileType              fileType;
    const QList<QUrl>&            inputFiles;
    const bool                          addGPlusMetadata;
    const QString&                      huginVersion;

public:

    CreatePtoTask(const QString& workDirPath, KIPIPanoramaPlugin::PanoramaFileType fileType,
                  QUrl& ptoUrl, const QList<QUrl>& inputFiles, const KIPIPanoramaPlugin::ItemUrlsMap& preProcessedMap,
                  bool addGPlusMetadata, const QString& huginVersion);
    ~CreatePtoTask();

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

};

}  // namespace KIPIPanoramaPlugin

#endif /* CREATEPTOTASK_H */
