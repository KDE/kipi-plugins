/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <klocale.h>
#include <kstandarddirs.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// LibKDcraw includes

#include <libkdcraw/dcrawinfocontainer.h>

// Local includes

#include "actions.h"
#include "rawdecodingiface.h"
#include "kphostsettings.h"
#include "kpmetadata.h"

namespace KIPIRawConverterPlugin
{

class ActionThread::Private
{
public:

    Private()
    {
    }

    KPSaveSettingsWidget::OutputFormat outputFormat;

    RawDecodingSettings                rawDecodingSettings;
};

//------------------------------------------------------------

class Task::Private
{
public:

    Private()
    {
        cancel                 = false;
        iface                  = 0;
        PluginLoader* const pl = PluginLoader::instance();
        if (pl)
        {
            iface = pl->interface();
        }
    }

    bool                               cancel;

    KUrl                               url;
    Action                             action;

    RawDecodingIface                   dcrawIface;

    Interface*                         iface;

    KPSaveSettingsWidget::OutputFormat outputFormat;

    RawDecodingSettings                rawDecodingSettings;
};

Task::Task(QObject* const parent, const KUrl& fileUrl, const Action& action)
    : Job(parent), d(new Private)
{
    d->url    = fileUrl;
    d->action = action;
}

Task::~Task()
{
    slotCancel();
    delete d;
}

void Task::setSettings(const RawDecodingSettings& rawDecodingSettings,
                       KPSaveSettingsWidget::OutputFormat outputFormat)
{
    d->rawDecodingSettings = rawDecodingSettings;
    d->outputFormat        = outputFormat;
}

void Task::slotCancel()
{
    d->cancel = true;
    d->dcrawIface.cancel();
}

void Task::run()
{
    if (d->cancel)
    {
        return;
    }

    switch (d->action)
    {
        case IDENTIFY:
        case IDENTIFY_FULL:
        {
            // Identify Camera model.
            DcrawInfoContainer info;
            {
                KPFileReadLocker(d->iface, d->url.toLocalFile());
                d->dcrawIface.rawFileIdentify(info, d->url.toLocalFile());
            }

            QString identify = i18n("Cannot identify RAW image");

            if (info.isDecodable)
            {
                if (d->action == IDENTIFY)
                {
                    identify = info.make + QString("-") + info.model;
                }
                else
                {
                    identify = i18n("<br>Make: %1<br>",   info.make);
                    identify.append(i18n("Model: %1<br>", info.model));

                    if (info.dateTime.isValid())
                    {
                        identify.append(i18n("Created: %1<br>",
                        KGlobal::locale()->formatDateTime(info.dateTime,
                                                          KLocale::ShortDate, true)));
                    }

                    if (info.aperture != -1.0)
                    {
                        identify.append(i18n("Aperture: f/%1<br>", QString::number(info.aperture)));
                    }

                    if (info.focalLength != -1.0)
                    {
                        identify.append(i18n("Focal: %1 mm<br>", info.focalLength));
                    }

                    if (info.exposureTime != -1.0)
                    {
                        identify.append(i18n("Exposure: 1/%1 s<br>", info.exposureTime));
                    }

                    if (info.sensitivity != -1)
                    {
                        identify.append(i18n("Sensitivity: %1 ISO", info.sensitivity));
                    }
                }
            }

            ActionData ad;
            ad.action  = d->action;
            ad.fileUrl = d->url;
            ad.message = identify;
            ad.success = true;
            emit signalFinished(ad);
            break;
        }

        case THUMBNAIL:
        {
            // Get embedded RAW file thumbnail.
            QImage image;
            {
                KPFileReadLocker(d->iface, d->url.toLocalFile());
                d->dcrawIface.loadRawPreview(image, d->url.toLocalFile());
            }

            ActionData ad;
            ad.action  = d->action;
            ad.fileUrl = d->url;
            ad.image   = image;
            ad.success = true;
            emit signalFinished(ad);
            break;
        }

        case PREVIEW:
        {
            ActionData ad1;
            ad1.action   = PREVIEW;
            ad1.fileUrl  = d->url;
            ad1.starting = true;
            emit signalStarting(ad1);

            QString destPath;
            bool result = false;
            {
                KPFileReadLocker(d->iface, d->url.toLocalFile());
                result = d->dcrawIface.decodeHalfRAWImage(d->url.toLocalFile(), destPath,
                                                          d->outputFormat, d->rawDecodingSettings);
            }

            ActionData ad2;
            ad2.action   = PREVIEW;
            ad2.fileUrl  = d->url;
            ad2.destPath = destPath;
            ad2.success  = result;
            emit signalFinished(ad2);
            break;
        }

        case PROCESS:
        {
            ActionData ad1;
            ad1.action   = PROCESS;
            ad1.fileUrl  = d->url;
            ad1.starting = true;
            emit signalStarting(ad1);

            QString destPath;
            bool result = false;

            {
                KPFileReadLocker(d->iface, d->url.toLocalFile());
                result = d->dcrawIface.decodeRAWImage(d->url.toLocalFile(), destPath,
                                                      d->outputFormat, d->rawDecodingSettings);
            }

            ActionData ad2;
            ad2.action   = PROCESS;
            ad2.fileUrl  = d->url;
            ad2.destPath = destPath;
            ad2.success  = result;
            emit signalFinished(ad2);
            break;
        }

        default:
        {
            qCritical() << "Unknown action specified";
        }
    }
}

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
