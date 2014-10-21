/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-12-31
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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


#ifndef TASK_H
#define TASK_H

// KDE includes

#include <kurl.h>
#include <threadweaver/Job.h>

// Libkdcraw includes

#include <libkdcraw/ractionthreadbase.h>

// Local includes

#include "timeadjustsettings.h"

using namespace KDcrawIface;
using namespace ThreadWeaver;

namespace KIPITimeAdjustPlugin
{

class Task : public Job
{
    Q_OBJECT

public:

    Task(QObject* const parent, const KUrl& url);
    ~Task();
    
    void setSettings(const TimeAdjustSettings& settings);
    void setItemsMap(QMap<KUrl, QDateTime> itemsMap);

Q_SIGNALS:

    void signalProcessStarted(const KUrl&);
    void signalProcessEnded(const KUrl&, int);

protected:

    void run();

public Q_SLOTS:

    void slotCancel();
    
private:

    class Private;
    Private* const d;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* TASK_H */
