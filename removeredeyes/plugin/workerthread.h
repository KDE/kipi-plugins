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

#include <ThreadWeaver/Job>

// KDE includes

#include <kurl.h>

// local includes

#include "actionthreadbase.h"
#include "commonsettings.h"
#include "savemethods.h"
#include "locator.h"
#include "workerthreaddata.h"

class QString;

using namespace KIPIPlugins;

namespace KIPIRemoveRedEyesPlugin
{

class WorkerThreadData;
struct WorkerThreadPriv;

class WorkerThread : public ActionThreadBase
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

    WorkerThread(QObject* parent, bool updateFileTimeStamp);
    ~WorkerThread();

    int  runType() const;
    void setRunType(int);

    void setSaveMethod(SaveMethod* method);
    void setLocator(Locator* locator);

    void loadSettings(const CommonSettings&);
    void setImagesList(const KUrl::List&);
    void setTempFile(const QString&, ImageType);

    void cancel();

private:

    WorkerThreadPriv* const pd;
};

// --------------------------------------------------------------------------

struct WorkerThreadPriv
{
    WorkerThreadPriv()
    {
        runtype             = WorkerThread::Testrun;
        cancel              = false;
        updateFileTimeStamp = false;
        saveMethod          = 0;
        locator             = 0;
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

class Task : public ThreadWeaver::Job
{
    Q_OBJECT

public:

    Task(const KUrl& url, QObject* parent = 0, WorkerThreadPriv* d = 0);

    const KUrl& url;

Q_SIGNALS:

    void calculationFinished(WorkerThreadData*);

private:

    WorkerThreadPriv* ld;

protected:

    void run();
};

} // namespace KIPIRemoveRedEyesPlugin

#endif // WORKERTHREAD_H
