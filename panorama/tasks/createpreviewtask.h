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

#ifndef CREATEPREVIEWTASK_H
#define CREATEPREVIEWTASK_H

// Local includes

#include "task.h"
#include "ptotype/ptotype.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

class CreatePreviewTask : public Task
{

private:

    QUrl&                               previewPtoUrl;
    QSharedPointer<const PTOType>       ptoData;
    const ItemUrlsMap                   preProcessedUrlsMap;

public:

    CreatePreviewTask(const QString& workDirPath, QSharedPointer<const PTOType> inputPTO, 
                      QUrl& previewPtoUrl, const ItemUrlsMap& preProcessedUrlsMap);
    ~CreatePreviewTask();

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

};

}  // namespace KIPIPanoramaPlugin

#endif /* CREATEPREVIEWTASK_H */
