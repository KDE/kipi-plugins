/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-06-08
 * Description : the calculation thread for red-eye removal
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

// Qt includes

#include <QMutex>

// KDE includes

#include <kurl.h>
#include <threadweaver/Job.h>

// Libkdcraw includes

#include <libkdcraw/ractionthreadbase.h>

// local includes

#include "commonsettings.h"
#include "savemethods.h"
#include "locator.h"
#include "workerthreaddata.h"

using namespace KDcrawIface;

namespace KIPIRemoveRedEyesPlugin
{

class WorkerThreadData;

class WorkerThread : public RActionThreadBase
{
    Q_OBJECT

public:

    enum RunType
    {
        Testrun = 0,
        Correction,
        Preview
    };

    enum ImageType
    {
        OriginalImage = 0,
        CorrectedImage,
        MaskImage
    };

Q_SIGNALS:

    void calculationFinished(WorkerThreadData*);

public:

    WorkerThread(QObject* const parent, bool updateFileTimeStamp);
    ~WorkerThread();

    int  runType() const;
    void setRunType(int);

    void setSaveMethod(SaveMethod* const method);
    void setLocator(Locator* const locator);

    void loadSettings(const CommonSettings&);
    void setImagesList(const KUrl::List&);
    void setTempFile(const QString&, ImageType);

    void cancel();

public:

    class Private;

private:

    Private* const pd;
};

// --------------------------------------------------------------------------

class WorkerThread::Private
{
public:

    Private()
    {
        runtype             = WorkerThread::Testrun;
        cancel              = false;
        updateFileTimeStamp = false;
        saveMethod          = 0;
        locator             = 0;
        progress            = 0;
    }

    bool           updateFileTimeStamp;
    bool           cancel;
    int            runtype;
    int            progress;

    CommonSettings settings;
    SaveMethod*    saveMethod;
    Locator*       locator;

    KUrl::List     urls;
    QString        maskPreviewFile;
    QString        correctedPreviewFile;
    QString        originalPreviewFile;

    QMutex         mutex;
};

// --------------------------------------------------------------------------

class Task : public Job
{
    Q_OBJECT

public:

    explicit Task(const KUrl& url, QObject* const parent = 0, WorkerThread::Private* const d = 0);

    const KUrl& url;

Q_SIGNALS:

    void calculationFinished(WorkerThreadData*);

protected:

    void run();

private:

    WorkerThread::Private* ld;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif // WORKERTHREAD_H
