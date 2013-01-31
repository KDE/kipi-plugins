/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-01
 * Description : Raw thumbnail thread.
 *
 * Copyright (C) 2004-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kprawthumbthread.moc"

// Qt includes

#include <QMutex>
#include <QWaitCondition>

// KDE includes

#include <kdeversion.h>
#include <kdebug.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

using namespace KDcrawIface;

namespace KIPIPlugins
{

class KPRawThumbThread::Private
{
public:

    Private()
    {
        size    = 256;
        running = false;
    }

    bool           running;

    int            size;

    QMutex         mutex;

    QWaitCondition condVar;

    KUrl::List     todo;
};

KPRawThumbThread::KPRawThumbThread(QObject* const parent, int size)
    : QThread(parent), d(new Private)
{
    d->size = size;
    start();
}

KPRawThumbThread::~KPRawThumbThread()
{
    cancel();
    wait();

    delete d;
}

void KPRawThumbThread::cancel()
{
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->running = false;
    d->condVar.wakeAll();
}

void KPRawThumbThread::getRawThumb(const KUrl& url)
{
    QMutexLocker lock(&d->mutex);
    d->todo << url;
    d->condVar.wakeAll();
}

void KPRawThumbThread::run()
{
    d->running = true;

    while (d->running)
    {
        KUrl url;

        QMutexLocker lock(&d->mutex);

        if (!d->todo.isEmpty())
            url = d->todo.takeFirst();
        else
            d->condVar.wait(&d->mutex);

        if (!url.isEmpty())
        {
            QImage img;
            bool ret = KDcraw::loadRawPreview(img, url.path());

            if (ret)
            {
                kDebug() << url << " :: processed as RAW file";
                emit signalRawThumb(url, img.scaled(d->size, d->size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            else
            {
                kDebug() << url << " :: not a RAW file";
                emit signalRawThumb(url, QImage());
            }
        }
    }
}

} // namespace KIPIPlugins
