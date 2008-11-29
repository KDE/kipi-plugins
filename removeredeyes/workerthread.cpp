/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the calculation thread for red-eye removal
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

// Local includes.

#include "eyelocator.h"
#include "removalsettings.h"
#include "simplesettings.h"
#include "storagesettingsbox.h"
#include "workerthreaddata.h"

namespace KIPIRemoveRedEyesPlugin
{

class WorkerThreadPriv
{
public:

    WorkerThreadPriv()
    {
        runtype = WorkerThread::TestRun;
        cancel  = false;
    }

    int                     runtype;
    bool                    cancel;
    RemovalSettings         settings;
    KUrl::List              urls;
};

WorkerThread::WorkerThread(QObject* parent)
            : QThread(parent),
              d(new WorkerThreadPriv)
{
}

WorkerThread::~ WorkerThread()
{
    // wait for the thread to finish
    wait();

    delete d;
}

void WorkerThread::run()
{
    int total = d->urls.count();

    if (total <= 0)
        return;

    int i = 1;
    d->cancel = false;

    for (KUrl::List::iterator it = d->urls.begin(); it != d->urls.end(); ++it, ++i)
    {
        KUrl& url = (KUrl&)(*it);
        if (!url.isLocalFile())
            break;

        // we need to convert the QString to const char* for openCV to work
        QByteArray src = QFile::encodeName(url.path());
        QByteArray cls = QFile::encodeName(d->settings.classifierFile);

        bool scaleDown = false;

        if (d->settings.simpleMode == SimpleSettings::Fast && d->settings.useSimpleMode)
            scaleDown = true;

        EyeLocator loc(src.data(),
                       cls.data(),
                       d->settings.scaleFactor,
                       d->settings.neighborGroups,
                       d->settings.minRoundness,
                       d->settings.minBlobsize,
                       scaleDown);

        // save image to specified location
        if ((d->runtype == Correction) && (loc.redEyes() > 0))
        {
            QFileInfo info(url.path());
            KUrl saveLocation = KUrl(info.path());

            switch (d->settings.storageMode)
            {
                case StorageSettingsBox::Subfolder:
                {
                    saveLocation.addPath(d->settings.subfolderName);

                    // check if subfolder exists
                    if (!QDir(saveLocation.path()).exists())
                        QDir(info.path()).mkdir(d->settings.subfolderName);

                    saveLocation.addPath(info.fileName());
                    break;
                }
                case StorageSettingsBox::Suffix:
                {
                    QString file = info.baseName();
                    file.append(d->settings.suffixName);
                    file.append(".");
                    file.append(info.suffix());
                    saveLocation.addPath(file);
                    break;
                }
                case StorageSettingsBox::Overwrite:
                {
                    saveLocation.addPath(info.fileName());
                    break;
                }
            }

            QByteArray dest = QFile::encodeName(saveLocation.path());
            loc.saveImage(dest.data());
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

} // namespace KIPIRemoveRedEyesPlugin
