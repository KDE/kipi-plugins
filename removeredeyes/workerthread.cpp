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

// Qt includes

#include <QString>

// KDE includes

#include <kdebug.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>
#include <libkexiv2/version.h>

// Local includes

#include "locator.h"
#include "savemethodfactory.h"
#include "savemethods.h"
#include "commonsettings.h"
#include "haarsettings.h"
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
        locator             = 0;
    }

    bool                updateFileTimeStamp;
    bool                cancel;
    int                 runtype;

    CommonSettings      settings;
    SaveMethod*         saveMethod;
    Locator*            locator;

    KUrl::List          urls;
    QString             maskPreviewFile;
    QString             correctedPreviewFile;
    QString             originalPreviewFile;
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
    if (!d->locator)
    {
        kDebug() << "no locator has been defined";
        return;
    }
    if (d->urls.count() <= 0) return;
    if (!d->saveMethod) return;


    // --------------------------------------------------------

    int i     = 1;
    d->cancel = false;

    for (KUrl::List::const_iterator it = d->urls.constBegin(); it != d->urls.constEnd(); ++it, ++i)
    {
        KUrl& url = (KUrl&)(*it);
        if (!url.isLocalFile())
            break;

        QString src  = url.path();
        int eyes     = 0;

        switch (d->runtype)
        {
            case Correction:
            {
                // backup metatdata
                KExiv2Iface::KExiv2 meta;
#if KEXIV2_VERSION >= 0x000600
                meta.setUpdateFileTimeStamp(d->updateFileTimeStamp);
#endif
                meta.load(src);

                // check if custom keyword should be added
                if (d->settings.addKeyword)
                {
                    QStringList oldKeywords = meta.getIptcKeywords();
                    QStringList newKeywords = meta.getIptcKeywords();
                    newKeywords.append(d->settings.keywordName);
                    meta.setIptcKeywords(oldKeywords, newKeywords);
                }

                // start correction
                QString dest = d->saveMethod->savePath(src, d->settings.extraName);
                eyes = d->locator->startCorrection(src, dest);

                // restore metadata
                meta.save(dest);
                break;
            }
            case Testrun: eyes = d->locator->startTestrun(src); break;
            case Preview: eyes = d->locator->startPreview(src); break;
        }

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

void WorkerThread::loadSettings(const CommonSettings& newSettings)
{
    d->settings = newSettings;
//    d->settings.addKeyword      = newSettings.addKeyword;
//    d->settings.storageMode     = newSettings.storageMode;
//    d->settings.unprocessedMode = newSettings.unprocessedMode;
//    d->settings.extraName       = newSettings.extraName;
//    d->settings.keywordName     = newSettings.keywordName;
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

void WorkerThread::setSaveMethod(SaveMethod* method)
{
    if (!method)
        return;

    d->saveMethod = method;
}

void WorkerThread::setLocator(Locator* locator)
{
    if (!locator)
        return;

    d->locator = locator;
}

} // namespace KIPIRemoveRedEyesPlugin
