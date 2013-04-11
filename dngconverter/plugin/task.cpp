/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-12-24
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#include "task.moc"

// Qt includes

#include <QFileInfo>

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
#include "kpmetadata.h"

using namespace KDcrawIface;
using namespace KIPIPlugins;

namespace KIPIDNGConverterPlugin
{

class Task::Private
{
public:

    Private()
    {
        backupOriginalRawFile  = false;
        compressLossLess       = true;
        updateFileDate         = false;
        cancel                 = false;
        previewMode            = DNGWriter::MEDIUM;
        iface                  = 0;
        PluginLoader* const pl = PluginLoader::instance();

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

    KUrl       url;
    Action     action;

    DNGWriter  dngProcessor;

    Interface* iface;
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

void Task::setBackupOriginalRawFile(bool b)
{
    d->backupOriginalRawFile = b;
}

void Task::setCompressLossLess(bool b)
{
    d->compressLossLess = b;
}

void Task::setUpdateFileDate(bool b)
{
    d->updateFileDate = b;
}

void Task::setPreviewMode(int mode)
{
    d->previewMode = mode;
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
        {
            // Identify Camera model.
            DcrawInfoContainer info;
            {
                 KPFileReadLocker(d->iface, d->url.toLocalFile());
                 KDcraw::rawFileIdentify(info, d->url.toLocalFile());
            }

            QString identify = i18n("Cannot identify Raw image");

            if (info.isDecodable)
            {
                identify = info.make + QString("-") + info.model;
            }

            ActionData ad;
            ad.action  = d->action;
            ad.fileUrl = d->url;
            ad.message = identify;
            ad.result  = DNGWriter::PROCESSCOMPLETE;
            emit signalFinished(ad);
            break;
        }

        case PROCESS:
        {
            ActionData ad1;
            ad1.action   = PROCESS;
            ad1.fileUrl  = d->url;
            ad1.starting = true;
            emit signalStarting(ad1);

            int     ret = DNGWriter::PROCESSCOMPLETE;
            QString destPath;

            {
                KPFileReadLocker(d->iface, d->url.toLocalFile());
                QFileInfo fi(d->url.toLocalFile());
                destPath = fi.absolutePath() + QString("/") + ".kipi-dngconverter-tmp-" +
                           QString::number(QDateTime::currentDateTime().toTime_t()) + QString(d->url.fileName());

                d->dngProcessor.reset();
                d->dngProcessor.setInputFile(d->url.toLocalFile());
                d->dngProcessor.setOutputFile(destPath);
                d->dngProcessor.setBackupOriginalRawFile(d->backupOriginalRawFile);
                d->dngProcessor.setCompressLossLess(d->compressLossLess);
                d->dngProcessor.setUpdateFileDate(d->updateFileDate);
                d->dngProcessor.setPreviewMode(d->previewMode);
                ret = d->dngProcessor.convert();
            }

            ActionData ad2;
            ad2.action   = PROCESS;
            ad2.fileUrl  = d->url;
            ad2.destPath = destPath;
            ad2.result   = ret;
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

void Task::slotCancel()
{
    d->cancel = true;
    d->dngProcessor.cancel();
}

}  // namespace KIPIDNGConverterPlugin

