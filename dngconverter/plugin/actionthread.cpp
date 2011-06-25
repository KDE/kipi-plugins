/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-24
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2008-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtDebug>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/dcrawinfocontainer.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "actions.h"
#include "dngwriter.h"

using namespace KDcrawIface;
using namespace DNGIface;

namespace KIPIDNGConverterPlugin
{

class ActionThread::ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        backupOriginalRawFile = false;
        compressLossLess      = true;
        updateFileDate        = false;
        running               = false;
        previewMode           = DNGWriter::MEDIUM;
    }

    class Task
    {
        public:

            KUrl   fileUrl;
            Action action;
    };

    bool           backupOriginalRawFile;
    bool           compressLossLess;
    bool           updateFileDate;
    bool           running;

    int            previewMode;

    QMutex         mutex;

    QWaitCondition condVar;

    QList<Task*>   todo;

    DNGWriter      dngProcessor;
};

ActionThread::ActionThread(QObject* parent)
            : QThread(parent), d(new ActionThreadPriv)
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
    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        ActionThreadPriv::Task* t = new ActionThreadPriv::Task;
        t->fileUrl                = *it;
        t->action                 = IDENTIFY;

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
    }
}

void ActionThread::processRawFiles(const KUrl::List& urlList)
{
    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        ActionThreadPriv::Task* t = new ActionThreadPriv::Task;
        t->fileUrl                = *it;
        t->action                 = PROCESS;

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
    }
}

void ActionThread::cancel()
{
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->running = false;
    d->condVar.wakeAll();
    d->dngProcessor.cancel();
}

void ActionThread::run()
{
    d->running = true;
    while (d->running)
    {
        ActionThreadPriv::Task* t = 0;
        {
            QMutexLocker lock(&d->mutex);
            if (!d->todo.isEmpty())
                t = d->todo.takeFirst();
            else
                d->condVar.wait(&d->mutex);
        }

        if (t)
        {
            switch (t->action)
            {
                case IDENTIFY:
                {
                    // Identify Camera model.
                    DcrawInfoContainer info;
                    KDcraw::rawFileIdentify(info, t->fileUrl.path());

                    QString identify = i18n("Cannot identify Raw image");
                    if (info.isDecodable)
                    {
                        identify = info.make + QString("-") + info.model;
                    }

                    ActionData ad;
                    ad.action  = t->action;
                    ad.fileUrl = t->fileUrl;
                    ad.message = identify;
                    ad.success = true;
                    emit finished(ad);
                    break;
                }

                case PROCESS:
                {
                    ActionData ad1;
                    ad1.action   = PROCESS;
                    ad1.fileUrl  = t->fileUrl;
                    ad1.starting = true;
                    emit starting(ad1);

                    QFileInfo fi(t->fileUrl.path());
                    QString destPath = fi.absolutePath() + QString("/") + ".kipi-dngconverter-tmp-"
                                     + QString::number(QDateTime::currentDateTime().toTime_t());

                    d->dngProcessor.reset();
                    d->dngProcessor.setInputFile(t->fileUrl.path());
                    d->dngProcessor.setOutputFile(destPath);
                    d->dngProcessor.setBackupOriginalRawFile(d->backupOriginalRawFile);
                    d->dngProcessor.setCompressLossLess(d->compressLossLess);
                    d->dngProcessor.setUpdateFileDate(d->updateFileDate);
                    d->dngProcessor.setPreviewMode(d->previewMode);
                    int ret = d->dngProcessor.convert();

                    ActionData ad2;
                    ad2.action   = PROCESS;
                    ad2.fileUrl  = t->fileUrl;
                    ad2.destPath = destPath;
                    ad2.success  = ret == 0 ? true : false;
                    emit finished(ad2);
                    break;
                }

                default:
                {
                    kError() << "KIPIDNGConverterPlugin:ActionThread: "
                             << "Unknown action specified";
                }
            }
        }

        delete t;
    }
}

}  // namespace KIPIDNGConverterPlugin
