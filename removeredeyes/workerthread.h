/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the calculation thread for red-eye removal
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

// Qt includes.

#include <QThread>

// KDE includes.

#include <kurl.h>

namespace KIPIRemoveRedEyesPlugin
{

class WorkerThreadPriv;
class WorkerThreadData;
class RemovalSettings;

class WorkerThread : public QThread
{

    Q_OBJECT

public:

    enum RunType
    {
        TestRun = 0,
        Correction
    };

signals:

    void calculationFinished(WorkerThreadData*);

public:

    WorkerThread(QObject* parent);
    ~WorkerThread();

    void setRunType(int);
    int runType() const;

    void loadSettings(RemovalSettings);
    void setImagesList(const KUrl::List&);

    void cancel();

    virtual void run();

private:

    WorkerThreadPriv* const d;
};
} // namespace KIPIRemoveRedEyesPlugin

#endif
