/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2012-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt Includes

#include <QFileInfo>

// KDE includes

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <kdebug.h>

// Local includes

#include "task.h"

namespace KIPITimeAdjustPlugin
{

class ActionThread::Private
{
public:
    // Settings from GUI.
    TimeAdjustSettings    settings;

    // Map of item urls and Updated Timestamps.
    QMap<KUrl, QDateTime> itemsMap;
};


ActionThread::ActionThread(QObject* const parent)
    : RActionThreadBase(parent), d(new Private)
{
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void ActionThread::setUpdatedDates(const QMap<KUrl, QDateTime>& map)
{
    d->itemsMap                     = map;
    JobCollection* const collection = new JobCollection();

    foreach (const KUrl& url, d->itemsMap.keys())
    {
        Task* const t = new Task(this, url);
        t->setSettings(d->settings);
        t->setItemsMap(map);

        connect(t, SIGNAL(signalProcessStarted(KUrl)),
                this, SIGNAL(signalProcessStarted(KUrl)));

        connect(t, SIGNAL(signalProcessEnded(KUrl,int)),
                this, SIGNAL(signalProcessEnded(KUrl,int)));

        connect(this, SIGNAL(signalCancelTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);

        collection->addJob(t);
     }

    appendJob(collection);
}

void ActionThread::setSettings(const TimeAdjustSettings& settings)
{
    d->settings = settings;
}

void ActionThread::cancel()
{
    if (isRunning())
        emit signalCancelTask();

    RActionThreadBase::cancel();
}
/** Static public method also called from GUI to update listview information about new filename
 *  computed with timeStamp.
 */
KUrl ActionThread::newUrl(const KUrl& url, const QDateTime& dt)
{
    if (!dt.isValid()) return KUrl();

    QFileInfo fi(url.path());

    QString newFileName = fi.baseName();
    newFileName += '-';
    newFileName += dt.toString(QString("yyyyMMddThhmmss"));
    newFileName += '.';
    newFileName += fi.completeSuffix();

    KUrl newUrl = url;
    newUrl.setFileName(newFileName);

    return newUrl;
}

}  // namespace KIPITimeAdjustPlugin
