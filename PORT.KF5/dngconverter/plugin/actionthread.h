/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// KDE includes

#include <kurl.h>
#include <threadweaver/Job.h>

// Libkdcraw includes

#include <libkdcraw/ractionthreadbase.h>

// Local includes

#include "settingswidget.h"
#include "actions.h"

using namespace ThreadWeaver;
using namespace KDcrawIface;

namespace KIPIDNGConverterPlugin
{

class ActionData;

class ActionThread : public RActionThreadBase
{
    Q_OBJECT

public:

    ActionThread(QObject* const parent);
    ~ActionThread();

    void setBackupOriginalRawFile(bool b);
    void setCompressLossLess(bool b);
    void setUpdateFileDate(bool b);
    void setPreviewMode(int mode);

    void identifyRawFile(const KUrl& url);
    void identifyRawFiles(const KUrl::List& urlList);

    void processRawFile(const KUrl& url);
    void processRawFiles(const KUrl::List& urlList);

    void cancel();

Q_SIGNALS:

    void signalStarting(const KIPIDNGConverterPlugin::ActionData& ad);
    void signalFinished(const KIPIDNGConverterPlugin::ActionData& ad);

    /** Signal to emit to sub-tasks to cancel processing.
     */
    void signalCancelTask();


private:

    class Private;
    Private* const d;
};
}  // namespace KIPIDNGConverterPlugin

#endif /* ACTIONTHREAD_H */
