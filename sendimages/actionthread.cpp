/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-09
 * Description : a class to manage actions using threads
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QMutexLocker>
#include <QtDebug>
#include <QMutex>
#include <QWaitCondition>

// Local includes.

#include "imageresize.h"
#include "actionthread.h"
#include "actionthread.moc"

namespace KIPISendimagesPlugin
{

class ActionThreadPriv
{
public:

    ActionThreadPriv()
    {
        count   = 0;
        running = false;
    }

    class Task
    {
        public:

            KUrl                   fileUrl;
            QString                destName;
            Action                 action;
            EmailSettingsContainer settings;
            
    };

    bool             running;
    
    int              count;

    QMutex           mutex;

    QWaitCondition   condVar;

    QList<Task*>     todo;
};

ActionThread::ActionThread(QObject *parent)
            : QThread(parent)
{
    d = new ActionThreadPriv;
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void ActionThread::resize(const EmailSettingsContainer& settings)
{
    d->count = 0;
    int i    = 1;

    for (QList<EmailItem>::const_iterator it = settings.itemsList.begin();
         it != settings.itemsList.end(); ++it) 
    {
        QString tmp;

        ActionThreadPriv::Task *t = new ActionThreadPriv::Task;
        t->fileUrl                = (*it).url; 
        t->action                 = Resize;
        t->settings               = settings;
        t->destName               = QString("%1.%2").arg(tmp.sprintf("%03i", i)).arg(t->settings.format().toLower());

        QMutexLocker lock(&d->mutex);
        d->todo << t;
        d->condVar.wakeAll();
        i++;
    }
}

void ActionThread::cancel()
{
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->running = false;
    d->count   = 0;
    d->condVar.wakeAll();
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
            QString errString;

            switch (t->action)
            {
                case Resize:
                {
                    emit starting(t->fileUrl, Resize);

                    bool result = true;
                    ImageResize imageResize(t->settings);
                    result      = imageResize.resize(t->fileUrl, t->destName, errString);

                    if (result)
                        emit finished(t->fileUrl, Resize);
                    else
                        emit failed(t->fileUrl, Resize, errString);

                    d->count++;
                    
                    if (t->settings.itemsList.count() == d->count)
                    {
                        emit complete(Resize);
                        d->count = 0;
                    }
                    break;
                }
                default:
                {
                    qCritical() << "KIPISendimagesPlugin:ActionThread: "
                                << "Unknown action specified"
                                << endl;
                }
            }

            delete t;
        }
    }
}

}  // NameSpace KIPISendimagesPlugin
