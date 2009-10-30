/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "actionthread.h"
#include "actionthread.moc"

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtDebug>

// KDE includes

#include <klocale.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/dcrawinfocontainer.h>

// Local includes

#include "actions.h"
#include "rawdecodingiface.h"

namespace KIPIRawConverterPlugin
{

class ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        running = false;
    }

    class Task
    {
        public:

            KUrl                             fileUrl;
            Action                           action;
            SaveSettingsWidget::OutputFormat outputFormat;
            KDcrawIface::RawDecodingSettings decodingSettings;
    };

    bool                             running;

    QMutex                           mutex;

    QWaitCondition                   condVar;

    QList<Task*>                     todo;

    RawDecodingIface                 dcrawIface;

    SaveSettingsWidget::OutputFormat outputFormat;

    KDcrawIface::RawDecodingSettings rawDecodingSettings;
};

ActionThread::ActionThread(QObject* parent, bool updateFileTimeStamp)
            : QThread(parent), d(new ActionThreadPriv)
{
    qRegisterMetaType<ActionData>();
    d->dcrawIface.setUpdateFileTimeStamp(updateFileTimeStamp);
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void ActionThread::setRawDecodingSettings(KDcrawIface::RawDecodingSettings rawDecodingSettings,
                                          SaveSettingsWidget::OutputFormat outputFormat)
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

void ActionThread::identifyRawFiles(const KUrl::List& urlList, bool full)
{
    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->fileUrl                = *it;
        t->action                 = full ? IDENTIFY_FULL : IDENTIFY;

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
    }
}

void ActionThread::thumbRawFile(const KUrl& url)
{
    KUrl::List oneFile;
    oneFile.append(url);
    thumbRawFiles(oneFile);
}

void ActionThread::thumbRawFiles(const KUrl::List& urlList)
{
    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->fileUrl                = *it;
        t->action                 = THUMBNAIL;

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
        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->fileUrl                = *it;
        t->outputFormat           = d->outputFormat;
        t->decodingSettings       = d->rawDecodingSettings;
        t->action                 = PROCESS;

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
    }
}

void ActionThread::processHalfRawFiles(const KUrl::List& urlList)
{
    for (KUrl::List::const_iterator it = urlList.constBegin();
         it != urlList.constEnd(); ++it )
    {
        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->fileUrl                = *it;
        t->outputFormat           = d->outputFormat;
        t->decodingSettings       = d->rawDecodingSettings;
        t->action                 = PREVIEW;

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
    d->dcrawIface.cancel();
}

void ActionThread::run()
{
    d->running = true;
    while (d->running)
    {
        ActionThreadPriv::Task *t = 0;
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
                case IDENTIFY_FULL:
                {
                    // Identify Camera model.
                    KDcrawIface::DcrawInfoContainer info;
                    d->dcrawIface.rawFileIdentify(info, t->fileUrl.path());

                    QString identify = i18n("Cannot identify RAW image");
                    if (info.isDecodable)
                    {
                        if (t->action == IDENTIFY)
                            identify = info.make + QString("-") + info.model;
                        else
                        {
                            identify = i18n("Make: %1\n", info.make);
                            identify.append(i18n("Model: %1\n", info.model));

                            if (info.dateTime.isValid())
                            {
                                identify.append(i18n("Created: %1\n",
                                         KGlobal::locale()->formatDateTime(info.dateTime,
                                                                           KLocale::ShortDate, true)));
                            }

                            if (info.aperture != -1.0)
                            {
                                identify.append(i18n("Aperture: f/%1\n", QString::number(info.aperture)));
                            }

                            if (info.focalLength != -1.0)
                            {
                                identify.append(i18n("Focal: %1 mm\n", info.focalLength));
                            }

                            if (info.exposureTime != -1.0)
                            {
                                identify.append(i18n("Exposure: 1/%1 s\n", info.exposureTime));
                            }

                            if (info.sensitivity != -1)
                            {
                                identify.append(i18n("Sensitivity: %1 ISO", info.sensitivity));
                            }
                        }
                    }

                    ActionData ad;
                    ad.action  = t->action;
                    ad.fileUrl = t->fileUrl;
                    ad.message = identify;
                    ad.success = true;
                    emit finished(ad);
                    break;
                }

                case THUMBNAIL:
                {
                    // Get embedded RAW file thumbnail.
                    QImage image;
                    d->dcrawIface.loadDcrawPreview(image, t->fileUrl.path());

                    ActionData ad;
                    ad.action  = t->action;
                    ad.fileUrl = t->fileUrl;
                    ad.image   = image;
                    ad.success = true;
                    emit finished(ad);
                    break;
                }

                case PREVIEW:
                {
                    ActionData ad1;
                    ad1.action   = PREVIEW;
                    ad1.fileUrl  = t->fileUrl;
                    ad1.starting = true;
                    emit starting(ad1);

                    QString destPath;
                    bool result = d->dcrawIface.decodeHalfRAWImage(t->fileUrl.path(), destPath,
                                                                   t->outputFormat, t->decodingSettings);

                    ActionData ad2;
                    ad2.action   = PREVIEW;
                    ad2.fileUrl  = t->fileUrl;
                    ad2.destPath = destPath;
                    ad2.success  = result;
                    emit finished(ad2);
                    break;
                }

                case PROCESS:
                {
                    ActionData ad1;
                    ad1.action   = PROCESS;
                    ad1.fileUrl  = t->fileUrl;
                    ad1.starting = true;
                    emit starting(ad1);

                    QString destPath;
                    bool result = d->dcrawIface.decodeRAWImage(t->fileUrl.path(), destPath,
                                                               t->outputFormat, t->decodingSettings);

                    ActionData ad2;
                    ad2.action   = PROCESS;
                    ad2.fileUrl  = t->fileUrl;
                    ad2.destPath = destPath;
                    ad2.success  = result;
                    emit finished(ad2);
                    break;
                }

                default:
                {
                    qCritical() << "KIPIRawConverterPlugin:ActionThread: "
                                << "Unknown action specified"
                                << endl;
                }
            }
        }

        delete t;
    }
}

}  // namespace KIPIRawConverterPlugin
