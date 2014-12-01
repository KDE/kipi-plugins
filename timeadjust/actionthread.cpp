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

#include "actionthread.h"

// Qt Includes

#include <QFileInfo>

// KDE includes

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
    QMap<QUrl, QDateTime> itemsMap;
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

void ActionThread::setUpdatedDates(const QMap<QUrl, QDateTime>& map)
{
    d->itemsMap = map;
    RJobCollection collection;

    foreach (const QUrl& url, d->itemsMap.keys())
    {
        Task* const t = new Task(url);
        t->setSettings(d->settings);
        t->setItemsMap(map);

        connect(t, SIGNAL(signalProcessStarted(QUrl)),
                this, SIGNAL(signalProcessStarted(QUrl)));

        connect(t, SIGNAL(signalProcessEnded(QUrl,int)),
                this, SIGNAL(signalProcessEnded(QUrl,int)));

        connect(this, SIGNAL(signalCancelTask()),
                t, SLOT(cancel()), Qt::QueuedConnection);

        collection.insert(t, 0);
     }

    appendJobs(collection);
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
QUrl ActionThread::newUrl(const QUrl& url, const QDateTime& dt)
{
    if (!dt.isValid()) return QUrl();

    QFileInfo fi(url.path());

    QString newFileName = fi.baseName();
    newFileName += '-';
    newFileName += dt.toString(QString("yyyyMMddThhmmss"));
    newFileName += '.';
    newFileName += fi.completeSuffix();

    QUrl newUrl = url;
    newUrl.setPath(newUrl.path() + newFileName);

    return newUrl;
}

}  // namespace KIPITimeAdjustPlugin
