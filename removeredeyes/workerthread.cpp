/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the calculation thread for red-eye removal
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "workerthread.h"
#include "workerthread.moc"

// Qt includes.

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>

// LibKExiv2 includes.

#include <libkexiv2/kexiv2.h>
#include <libkexiv2/version.h>

// Local includes.

#include "SaveMethods.h"
#include "eyelocator.h"
#include "removalsettings.h"
#include "simplesettings.h"
#include "storagesettingsbox.h"
#include "workerthreaddata.h"

namespace KIPIRemoveRedEyesPlugin
{

struct WorkerThreadPriv
{
    WorkerThreadPriv()
    {
        runtype             = WorkerThread::Testrun;
        cancel              = false;
        updateFileTimeStamp = false;
        saveMethod          = 0;
    }

    bool                updateFileTimeStamp;
    bool                cancel;
    int                 runtype;

    QString             maskPreviewFile;
    QString             correctedPreviewFile;
    QString             originalPreviewFile;

    KUrl::List          urls;

    RemovalSettings     settings;
    SaveMethodAbstract* saveMethod;
};

WorkerThread::WorkerThread(QObject* parent, bool updateFileTimeStamp)
            : QThread(parent), d(new WorkerThreadPriv)
{
    d->updateFileTimeStamp = updateFileTimeStamp;
}

WorkerThread::~ WorkerThread()
{
    // wait for the thread to finish
    wait();

    if (d->saveMethod)
        delete d->saveMethod;
    delete d;
}

void WorkerThread::run()
{
    int total = d->urls.count();

    if (total <= 0)
        return;

    if (!d->saveMethod)
        return;

    int i = 1;
    d->cancel = false;

    for (KUrl::List::const_iterator it = d->urls.constBegin(); it != d->urls.constEnd(); ++it, ++i)
    {
        KUrl& url = (KUrl&)(*it);
        if (!url.isLocalFile())
            break;

        QString src = url.path();
        QString cls = d->settings.classifierFile;

        bool scaleDown = false;

        if (d->settings.simpleMode == SimpleSettings::Fast && d->settings.useSimpleMode)
            scaleDown = true;

        // The EyeLocator object will detect and remove the red-eye effect
        EyeLocator loc(src, cls);
        loc.setScaleFactor(d->settings.scaleFactor);
        loc.setNeighborGroups(d->settings.neighborGroups);
        loc.setMinRoundness(d->settings.minRoundness);
        loc.setMinBlobsize(d->settings.minBlobsize);

        // start correction
        loc.startCorrection(scaleDown);

        // generate save-location path
        if ((d->runtype == Correction) && (loc.redEyes() > 0))
        {
            // backup metatdata
            KExiv2Iface::KExiv2 meta;
#if KEXIV2_VERSION >= 0x000600
            meta.setUpdateFileTimeStamp(d->updateFileTimeStamp);
#endif
            meta.load(url.path());

            // check if custom keyword should be added
            if (d->settings.addKeyword)
            {
                QStringList oldKeywords = meta.getIptcKeywords();
                QStringList newKeywords = meta.getIptcKeywords();
                newKeywords.append(d->settings.keywordName);
                meta.setIptcKeywords(oldKeywords, newKeywords);
            }

            // save image
            QString dest = d->saveMethod->savePath(url.path(), d->settings.extraName);
            loc.saveImage(dest, EyeLocator::Final);

            // restore metadata
            meta.save(d->saveMethod->savePath(url.path(), d->settings.extraName));
        }

        if (d->runtype == Preview)
        {
            // save preview files in KDE temp dir
            loc.saveImage(d->originalPreviewFile,  EyeLocator::OriginalPreview);
            loc.saveImage(d->correctedPreviewFile, EyeLocator::CorrectedPreview);
            loc.saveImage(d->maskPreviewFile,      EyeLocator::MaskPreview);
        }

        int eyes = loc.redEyes();
        emit calculationFinished(new WorkerThreadData(url, i, eyes));

        if (d->cancel)
            break;
    }
}

void WorkerThread::setRunType(int type)
{
    d->runtype = type;
}

int WorkerThread::runType() const
{
    return d->runtype;
}

void WorkerThread::cancel()
{
    d->cancel = true;
}

void WorkerThread::loadSettings(RemovalSettings newSettings)
{
    d->settings = newSettings;
}

void WorkerThread::setImagesList(const KUrl::List& list)
{
    d->urls = list;
}

void WorkerThread::setTempFile(const QString& temp, ImageType type)
{
    switch (type)
    {
        case OriginalImage:
            d->originalPreviewFile = temp;
            break;

        case CorrectedImage:
            d->correctedPreviewFile = temp;
            break;

        case MaskImage:
            d->maskPreviewFile = temp;
            break;
    }
}

void WorkerThread::setSaveMethod(SaveMethodAbstract* method)
{
    if (!method)
        return;

    if (d->saveMethod)
        delete d->saveMethod;
    d->saveMethod = method;
}

} // namespace KIPIRemoveRedEyesPlugin
