/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : prints debugging messages about the thread activity in action thread class
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#ifndef WEAVEROBSERVERTEST_H
#define WEAVEROBSERVERTEST_H

// KDE includes

#include <threadweaver/Job.h>
#include <threadweaver/WeaverObserver.h>
#include <threadweaver/State.h>
#include <threadweaver/Thread.h>

/** WeaverObserverTest is a simple test of the actionThread class that
    prints debug messages when signals are received.
*/
class WeaverObserverTest : public ThreadWeaver::WeaverObserver
{
    Q_OBJECT

public:

    WeaverObserverTest(QObject* parent=0);

protected Q_SLOTS:

    void slotWeaverStateChanged(ThreadWeaver::State*);
    void slotThreadStarted(ThreadWeaver::Thread*);
    void slotThreadBusy(ThreadWeaver::Thread*, ThreadWeaver::Job*);
    void slotThreadSuspended(ThreadWeaver::Thread*);
    void slotThreadExited(ThreadWeaver::Thread*);
};

#endif // WEAVEROBSERVERTEST_H
