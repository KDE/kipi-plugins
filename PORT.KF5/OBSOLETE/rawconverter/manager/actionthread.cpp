/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
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

// Local includes

#include "task.h"

namespace KIPIRawConverterPlugin
{

class ActionThread::Private
{
public:

    Private()
    {
        outputFormat = KPSaveSettingsWidget::OUTPUT_PNG;
    }

    KPSaveSettingsWidget::OutputFormat outputFormat;

    RawDecodingSettings                rawDecodingSettings;
};

//------------------------------------------------------------

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

void ActionThread::setSettings(const RawDecodingSettings& rawDecodingSettings,
                               KPSaveSettingsWidget::OutputFormat outputFormat)
{
    d->rawDecodingSettings = rawDecodingSettings;
    d->outputFormat        = outputFormat;
}

void ActionThread::processRawFile(const KUrl& url)
{
    KUrl::List oneFile;
    oneFile.append(url);
    processRawFiles(oneFile);
}

void ActionThread::processHalfRawFile(const KUrl& url)
{
    KUrl::List oneFile;
    oneFile.append(url);
    processHalfRawFiles(oneFile);
}

void ActionThread::identifyRawFile(const KUrl& url, bool full)
{
    KUrl::List oneFile;
    oneFile.append(url);
    identifyRawFiles(oneFile, full);
}

void ActionThread::thumbRawFile(const KUrl& url)
{
    KUrl::List oneFile;
    oneFile.append(url);
    thumbRawFiles(oneFile);
}

void ActionThread::identifyRawFiles(const KUrl::List& urlList, bool full)
{
    JobCollection* const collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* const t = new Task(this, *it, full ? IDENTIFY_FULL : IDENTIFY);
        t->setSettings(d->rawDecodingSettings, d->outputFormat);

        connect(t, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)));

        connect(this, SIGNAL(signalCancelTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::thumbRawFiles(const KUrl::List& urlList)
{
    JobCollection* const collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* const t = new Task(this, *it, THUMBNAIL);
        t->setSettings(d->rawDecodingSettings, d->outputFormat);

        connect(t, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)));

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
        t->setSettings(d->rawDecodingSettings, d->outputFormat);

        connect(t, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)));

        connect(this, SIGNAL(signalCancelTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::processHalfRawFiles(const KUrl::List& urlList)
{
    JobCollection* const collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* const t = new Task(this, *it, PREVIEW);
        t->setSettings(d->rawDecodingSettings, d->outputFormat);

        connect(t, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)));

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

}  // namespace KIPIRawConverterPlugin
