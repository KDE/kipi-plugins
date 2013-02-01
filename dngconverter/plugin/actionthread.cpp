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

#include "actionthread.moc"

// KDE includes

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/dcrawinfocontainer.h>
#include <libkdcraw/kdcraw.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "task.h"
#include "kpmetadata.h"

using namespace KDcrawIface;
using namespace KIPIPlugins;

namespace KIPIDNGConverterPlugin
{

class ActionThread::Private
{
public:

    Private()
    {
        backupOriginalRawFile = false;
        compressLossLess      = true;
        updateFileDate        = false;
        previewMode           = DNGWriter::MEDIUM;
    }

    bool       backupOriginalRawFile;
    bool       compressLossLess;
    bool       updateFileDate;
    int        previewMode;
};

ActionThread::ActionThread(QObject* const parent)
    : RActionThreadBase(parent), d(new Private)
{
    qRegisterMetaType<ActionData>();
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void ActionThread::setBackupOriginalRawFile(bool b)
{
    d->backupOriginalRawFile = b;
}

void ActionThread::setCompressLossLess(bool b)
{
    d->compressLossLess = b;
}

void ActionThread::setUpdateFileDate(bool b)
{
    d->updateFileDate = b;
}

void ActionThread::setPreviewMode(int mode)
{
    d->previewMode = mode;
}

void ActionThread::processRawFile(const KUrl& url)
{
    KUrl::List oneFile;
    oneFile.append(url);
    processRawFiles(oneFile);
}

void ActionThread::identifyRawFile(const KUrl& url)
{
    KUrl::List oneFile;
    oneFile.append(url);
    identifyRawFiles(oneFile);
}

void ActionThread::identifyRawFiles(const KUrl::List& urlList)
{
    JobCollection* const collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* const t = new Task(this, *it, IDENTIFY);
        t->setBackupOriginalRawFile(d->backupOriginalRawFile);
        t->setCompressLossLess(d->compressLossLess);
        t->setUpdateFileDate(d->updateFileDate);
        t->setPreviewMode(d->previewMode);

        connect(t, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)));

        connect(this, SIGNAL(signalCancelTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::processRawFiles(const KUrl::List& urlList)
{
    JobCollection* const collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* const t = new Task(this, *it, PROCESS);
        t->setBackupOriginalRawFile(d->backupOriginalRawFile);
        t->setCompressLossLess(d->compressLossLess);
        t->setUpdateFileDate(d->updateFileDate);
        t->setPreviewMode(d->previewMode);

        connect(t, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)));

        connect(this, SIGNAL(signalCancelTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::cancel()
{
    if (isRunning())
        emit signalCancelTask();

    RActionThreadBase::cancel();
}

}  // namespace KIPIDNGConverterPlugin
