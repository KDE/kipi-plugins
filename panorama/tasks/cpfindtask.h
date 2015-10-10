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

#ifndef CPFINDTASK_H
#define CPFINDTASK_H

// Local includes

#include "commandtask.h"



namespace KIPIPanoramaPlugin
{

class CpFindTask : public CommandTask
{

private:

    QUrl&                               cpFindPtoUrl;
    const bool                          celeste;
    const QUrl&                         ptoUrl;

public:

    CpFindTask(const QString& workDirPath, const QUrl& input,
               QUrl& cpFindUrl, bool celeste, const QString& cpFindPath);
    ~CpFindTask();

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) override;

};

}  // namespace KIPIPanoramaPlugin

#endif /* CPFINDTASK_H */
