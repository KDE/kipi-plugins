/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ACTION_THREAD_H
#define ACTION_THREAD_H

// Qt includes

#include <QObject>
#include <QThread>
#include <QDateTime>

// KDE includes

#include <kurl.h>

// Local includes

#include "kpactionthreadbase.h"

using namespace KIPIPlugins;
using namespace ThreadWeaver;

namespace KIPITimeAdjustPlugin
{

class ActionThread : public KPActionThreadBase
{
    Q_OBJECT

public:

    ActionThread(QObject* const parent);
    ~ActionThread();

    void setImages(const KUrl::List& urlList);
    void setDateSelection(bool useCustomDateBtn, const QDateTime& customTime, const QList<QDateTime>& imageOriginalDates);
    void setAppDateCheck(bool updAppDate);
    void setFileNameCheck(bool updFileName);
    void setEXIFDataCheck(bool updEXIFModDate, bool updEXIFOriDate, bool updEXIFDigDate);
    void setIPTCDateCheck(bool updIPTCDate);
    void setXMPDateCheck(bool updXMPDate);
    void setFileModDateCheck(bool updFileModDate);
    void cancel();

Q_SIGNALS:

    void signalProgressChanged(int);
    void signalErrorFilesUpdate(const QString&, const QString&);

public:

    class ActionThreadPriv;

private:

    ActionThreadPriv* const pd;
};

// ----------------------------------------------------------------------------------------------------

class Task : public Job
{
    Q_OBJECT

public:

    Task(QObject* const parent = 0, const KUrl& fileUrl = KUrl(), const QDateTime& dateTime = QDateTime(), 
         ActionThread::ActionThreadPriv* const d = 0);

Q_SIGNALS:

    void signalProgressChanged(int);
    void signalErrorFilesUpdate(const QString&, const QString&);

protected:

    void run();

private:

    KUrl                            url;
    QDateTime                       dateTime;

    ActionThread::ActionThreadPriv* ld;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* ACTION_THREAD_H */
