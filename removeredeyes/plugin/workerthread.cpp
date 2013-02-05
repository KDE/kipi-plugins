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

#include "workerthread.moc"

// Qt includes

#include <QString>

// KDE includes

#include <kdebug.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>

// Local includes

#include "savemethodfactory.h"
#include "haarsettings.h"
#include "simplesettings.h"
#include "storagesettingsbox.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIRemoveRedEyesPlugin
{

Task::Task(const KUrl& urli, QObject* const parent, WorkerThread::Private* const d)
    : Job(parent), url(urli)
{
    this->ld = d;
}

void Task::run()
{

    if (ld->cancel)
    {
        return;
    }

    if (!ld->locator)
    {
        kDebug() << "no locator has been defined";
        return;
    }

    if (!ld->saveMethod)
    {
        return;
    }

    if (!url.isLocalFile())
    {
        return;
    }

    QString src = url.path();
    int eyes    = 0;

    switch (ld->runtype)
    {
        case WorkerThread::Correction:
        {
            // backup metadata
            KPMetadata meta(src);

            // check if custom keyword should be added
            if (ld->settings.addKeyword)
            {
                QStringList oldKeywords = meta.getIptcKeywords();
                QStringList newKeywords = meta.getIptcKeywords();
                newKeywords.append(ld->settings.keywordName);
                meta.setIptcKeywords(oldKeywords, newKeywords);
            }

            // start correction
            ld->mutex.lock();
            QString dest = ld->saveMethod->savePath(src, ld->settings.extraName);
            eyes         = ld->locator->startCorrection(src, dest);
            ld->mutex.unlock();

            // restore metadata
            meta.save(dest);
            break;
        }
        case WorkerThread::Testrun:
            ld->mutex.lock();
            eyes = ld->locator->startTestrun(src);
            ld->mutex.unlock();
            break;
        case WorkerThread::Preview:
            ld->mutex.lock();
            eyes = ld->locator->startPreview(src);
            ld->mutex.unlock();
            break;
    }

    ld->progress++;

    emit calculationFinished(new WorkerThreadData(url, ld->progress, eyes));
}

// ----------------------------------------------------------------------------------------------------

WorkerThread::WorkerThread(QObject* const parent, bool updateFileTimeStamp)
    : RActionThreadBase(parent), pd(new Private)
{
    pd->updateFileTimeStamp = updateFileTimeStamp;
}

WorkerThread::~ WorkerThread()
{
    wait();

    delete pd->saveMethod;
    delete pd;
}

void WorkerThread::setRunType(int type)
{
    pd->runtype = type;
}

int WorkerThread::runType() const
{
    return pd->runtype;
}

void WorkerThread::cancel()
{
    pd->cancel = true;

    RActionThreadBase::cancel();
}

void WorkerThread::loadSettings(const CommonSettings& newSettings)
{
    pd->settings = newSettings;
    //    d->settings.addKeyword      = newSettings.addKeyword;
    //    d->settings.storageMode     = newSettings.storageMode;
    //    d->settings.unprocessedMode = newSettings.unprocessedMode;
    //    d->settings.extraName       = newSettings.extraName;
    //    d->settings.keywordName     = newSettings.keywordName;
}

void WorkerThread::setImagesList(const KUrl::List& list)
{
    pd->urls                        = list;
    JobCollection* const collection = new JobCollection(this);

    for (KUrl::List::const_iterator it = pd->urls.constBegin(); it != pd->urls.constEnd(); ++it)
    {
        Task* const t = new Task((KUrl&)(*it), this, pd);

        connect(t, SIGNAL(calculationFinished(WorkerThreadData*)),
                this, SIGNAL(calculationFinished(WorkerThreadData*)));

        collection->addJob(t);
    }

    appendJob(collection);
    pd->cancel   = false;
    pd->progress = 0;
}

void WorkerThread::setTempFile(const QString& temp, ImageType type)
{
    switch (type)
    {
        case OriginalImage:
            pd->originalPreviewFile = temp;
            break;

        case CorrectedImage:
            pd->correctedPreviewFile = temp;
            break;

        case MaskImage:
            pd->maskPreviewFile = temp;
            break;
    }
}

void WorkerThread::setSaveMethod(SaveMethod* const method)
{
    if (!method)
    {
        return;
    }

    pd->saveMethod = method;
}

void WorkerThread::setLocator(Locator* const locator)
{
    if (!locator)
    {
        return;
    }

    pd->locator = locator;
}

} // namespace KIPIRemoveRedEyesPlugin
