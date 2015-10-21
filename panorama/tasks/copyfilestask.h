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

#ifndef COPYFILESTASK_H
#define COPYFILESTASK_H

// Qt includes

#include <QPointer>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "task.h"

using namespace KIPI;

namespace KIPIPanoramaPlugin
{

class CopyFilesTask : public Task
{

public:

    CopyFilesTask(const QString& workDirPath, const QUrl& panoUrl, const QUrl& finalPanoUrl,
                    const QUrl& ptoUrl, const ItemUrlsMap& urls, bool sPTO, bool GPlusMetadata);
    ~CopyFilesTask();

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

private:

    const QUrl&                         panoUrl;
    const QUrl                          finalPanoUrl;
    const QUrl&                         ptoUrl;
    const ItemUrlsMap* const            urlList;
    const bool                          savePTO;
    const bool                          addGPlusMetadata;
    
    Interface*                          m_iface;
    QPointer<MetadataProcessor>         m_meta;
};

}  // namespace KIPIPanoramaPlugin

#endif /* COPYFILESTASK_H */
