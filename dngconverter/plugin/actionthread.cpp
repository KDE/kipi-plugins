/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtDebug>

// KDE includes

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/dcrawinfocontainer.h>
#include <libkdcraw/kdcraw.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "actions.h"
#include "dngwriter.h"
#include "kpmetadata.h"

using namespace KDcrawIface;
using namespace KIPIPlugins;
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
        cancel                = false;
        previewMode           = DNGWriter::MEDIUM;
        iface                 = 0;
        PluginLoader* pl      = PluginLoader::instance();
        if (pl)
        {
            iface = pl->interface();
        }
    }

    bool       backupOriginalRawFile;
    bool       compressLossLess;
    bool       updateFileDate;
    bool       cancel;

    int        previewMode;

    QMutex     mutex;

    DNGWriter  dngProcessor;

    Interface* iface;
};


Task::Task(QObject* const parent, const KUrl& fileUrl, const Action& action, ActionThread::ActionThreadPriv* const d)
    : Job(parent)
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
        {
            // Identify Camera model.
            DcrawInfoContainer info;
            {
                 KPFileReadLocker(m_d->iface, m_url.toLocalFile());
                 KDcraw::rawFileIdentify(info, m_url.toLocalFile());
            }

            QString identify = i18n("Cannot identify Raw image");
            if (info.isDecodable)
            {
                identify = info.make + QString("-") + info.model;
            }

            ActionData ad;
            ad.action  = m_action;
            ad.fileUrl = m_url;
            ad.message = identify;
            ad.success = true;
            emit signalFinished(ad);
            break;
        }

        case PROCESS:
        {
            ActionData ad1;
            ad1.action   = PROCESS;
            ad1.fileUrl  = m_url;
            ad1.starting = true;
            emit signalStarting(ad1);

            int     ret = 0;
            QString destPath;

            {
                KPFileReadLocker(m_d->iface, m_url.toLocalFile());
                QFileInfo fi(m_url.toLocalFile());
                destPath = fi.absolutePath() + QString("/") + ".kipi-dngconverter-tmp-" +
                        QString::number(QDateTime::currentDateTime().toTime_t()) + QString(m_url.fileName());

                m_d->dngProcessor.reset();
                m_d->dngProcessor.setInputFile(m_url.toLocalFile());
                m_d->dngProcessor.setOutputFile(destPath);
                m_d->dngProcessor.setBackupOriginalRawFile(m_d->backupOriginalRawFile);
                m_d->dngProcessor.setCompressLossLess(m_d->compressLossLess);
                m_d->dngProcessor.setUpdateFileDate(m_d->updateFileDate);
                m_d->dngProcessor.setPreviewMode(m_d->previewMode);
                ret = m_d->dngProcessor.convert();
            }

            ActionData ad2;
            ad2.action   = PROCESS;
            ad2.fileUrl  = m_url;
            ad2.destPath = destPath;
            ad2.success  = (ret == 0) ? true : false;
            emit signalFinished(ad2);
            break;
        }

        default:
        {
            kError() << "Unknown action specified";
            break;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------------

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
    JobCollection* collection = new JobCollection();

    for (KUrl::List::const_iterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        Task* t = new Task(this, *it, IDENTIFY, d);

        connect(t, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)));

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

        connect(t, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)),
                this, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)));

        connect(t, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)),
                this, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)));

        collection->addJob(t);
    }

    appendJob(collection);
}

void ActionThread::cancel()
{
    d->cancel = true;
    KPActionThreadBase::cancel();
}

}  // namespace KIPIDNGConverterPlugin
