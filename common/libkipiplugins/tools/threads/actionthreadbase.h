/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : re-implementation of action thread using threadweaver
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ACTIONTHREADBASE_H
#define ACTIONTHREADBASE_H

// Qt includes

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

// KDE includes

#include <kurl.h>
#include <ThreadWeaver/JobCollection>
#include <ThreadWeaver/Weaver>

//Local includes
#include "weaverobservertest.h"

class ActionThreadBase : public QThread
{
    Q_OBJECT

public:

    ActionThreadBase(QObject* parent=0);
    ~ActionThreadBase();

    void cancel();

protected:

    void run();

private Q_SLOTS:

    void slotFinished();

protected:

    class ActionThreadBasePriv;
    ActionThreadBasePriv* const d;
};

class ActionThreadBase::ActionThreadBasePriv
{
public:

    ActionThreadBasePriv()
    {
        running       = false;
        weaverRunning = false;
        log           = 0;
    }

    bool                  running;
    bool                  weaverRunning;

    QMutex                mutex;

    QWaitCondition        condVar;
    QWaitCondition        condVarJobs;

    QList<ThreadWeaver::JobCollection*> todo;

    ThreadWeaver::Weaver*                weaver;
    WeaverObserverTest*   log;
};

#endif // ACTIONTHREAD_H
