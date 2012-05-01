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

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtDebug>

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

class ActionThread::ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        cancel           = false;
        iface            = 0;
        PluginLoader* pl = PluginLoader::instance();
        if (pl)
        {
            iface = pl->interface();
        }
    }

    bool                               cancel;

    QMutex                             mutex;

    RawDecodingIface                   dcrawIface;

    KPSaveSettingsWidget::OutputFormat outputFormat;

    RawDecodingSettings                rawDecodingSettings;

    Interface*                         iface;
};

//------------------------------------------------------

Task::Task(QObject* const parent, const KUrl& fileUrl, const Action& action,
           ActionThread::ActionThreadPriv* const d): Job(parent)
{
    m_url    = fileUrl;
    m_action = action;
    m_d      = d;
}

Task::~Task()
{
}

void Task::run()
{
    if (m_d->cancel)
    {
        return;
    }

    switch (m_action)
    {
        case IDENTIFY:
        case IDENTIFY_FULL:
        {
            // Identify Camera model.
            DcrawInfoContainer info;
            {
                KPFileReadLocker(m_d->iface, m_url.toLocalFile());
                m_d->dcrawIface.rawFileIdentify(info, m_url.toLocalFile());
            }

            QString identify = i18n("Cannot identify RAW image");
            if (info.isDecodable)
            {
                if (m_action == IDENTIFY)
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
            ad.action  = m_action;
            ad.fileUrl = m_url;
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
                KPFileReadLocker(m_d->iface, m_url.toLocalFile());
                m_d->dcrawIface.loadDcrawPreview(image, m_url.toLocalFile());
            }

            ActionData ad;
            ad.action  = m_action;
            ad.fileUrl = m_url;
            ad.image   = image;
            ad.success = true;
            emit signalFinished(ad);
            break;
        }

        case PREVIEW:
        {
            ActionData ad1;
            ad1.action   = PREVIEW;
            ad1.fileUrl  = m_url;
            ad1.starting = true;
            emit signalStarting(ad1);

            QString destPath;
            bool result = false;
            {
                KPFileReadLocker(m_d->iface, m_url.toLocalFile());
                result = m_d->dcrawIface.decodeHalfRAWImage(m_url.toLocalFile(), destPath,
                                                            m_d->outputFormat, m_d->rawDecodingSettings);
            }

            ActionData ad2;
            ad2.action   = PREVIEW;
            ad2.fileUrl  = m_url;
            ad2.destPath = destPath;
            ad2.success  = result;
            emit signalFinished(ad2);
            break;
        }

        case PROCESS:
        {
            ActionData ad1;
            ad1.action   = PROCESS;
            ad1.fileUrl  = m_url;
            ad1.starting = true;
            emit signalStarting(ad1);

            QString destPath;
            bool result = false;

            {
                KPFileReadLocker(m_d->iface, m_url.toLocalFile());
                result = m_d->dcrawIface.decodeRAWImage(m_url.toLocalFile(), destPath,
                                                        m_d->outputFormat, m_d->rawDecodingSettings);
            }

            ActionData ad2;
            ad2.action   = PROCESS;
            ad2.fileUrl  = m_url;
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
    : KPActionThreadBase(parent), d(new ActionThreadPriv)
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

void ActionThread::setRawDecodingSettings(RawDecodingSettings rawDecodingSettings,
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
    JobCollection* collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* t = new Task(this, *it, full ? IDENTIFY_FULL : IDENTIFY, d);

        connect(t, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)));

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::thumbRawFiles(const KUrl::List& urlList)
{
    JobCollection* collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* t = new Task(this, *it, THUMBNAIL, d);

        connect(t, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)));

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::processRawFiles(const KUrl::List& urlList)
{
    JobCollection* collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* t = new Task(this, *it, PROCESS, d);

        connect(t, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)));

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::processHalfRawFiles(const KUrl::List& urlList)
{
    JobCollection* collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* t = new Task(this, *it, PREVIEW, d);

        connect(t, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)));

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::cancel()
{
    d->dcrawIface.cancel();
    d->cancel = true;
    KPActionThreadBase::cancel();
}

}  // namespace KIPIRawConverterPlugin
