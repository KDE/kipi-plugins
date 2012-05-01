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
#include <QDateTime>
#include <QMutex>
#include <QMap>

// KDE includes

#include <kurl.h>

// Local includes

#include "timeadjustsettings.h"
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

    void setUpdatedDates(const QMap<KUrl, QDateTime>& map);
    void setSettings(const TimeAdjustSettings& settings);
    void cancel();

    static KUrl newUrl(const KUrl& url, const QDateTime& dt);

Q_SIGNALS:

    void signalProgressChanged(int);
    void signalProcessStarted(const KUrl&);
    void signalProcessEnded(const KUrl&, int);

public:

    class ActionThreadPriv;

private:

    ActionThreadPriv* const d;
};

// ----------------------------------------------------------------------------------------------------

class Task : public Job
{
    Q_OBJECT

public:

    Task(QObject* const parent, const KUrl& url, ActionThread::ActionThreadPriv* const d);
    ~Task();

Q_SIGNALS:

    void signalProgressChanged(int);
    void signalProcessStarted(const KUrl&);
    void signalProcessEnded(const KUrl&, int);

protected:

    void run();

private:

    QMutex                          m_mutex;
    KUrl                            m_url;
    ActionThread::ActionThreadPriv* m_d;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* ACTION_THREAD_H */
