/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  : 2014-10-17
 * @brief : a class to manage image rotation using threads
 *
 * @author Copyright (C) 2011-2016 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2014 by Veaceslav Munteanu
 *         <a href="mailto:veaceslav dot munteanu90 at gmail dot com">veaceslav dot munteanu90 at gmail dot com</a>
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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes

#include <QUrl>

// Local includes

#include "kpthreadmanager.h"

using namespace KIPIPlugins;

class ActionThread : public KPThreadManager
{
    Q_OBJECT

public:

    ActionThread(QObject* const parent);
    ~ActionThread();

    void rotate(const QList<QUrl>& list);

Q_SIGNALS:

    void starting(const QUrl& url);
    void finished(const QUrl& url);
    void failed(const QUrl& url, const QString& err);
    void progress(const QUrl& url, int percent);

private Q_SLOTS:

    void slotJobDone();
    void slotJobProgress(int);
    void slotJobStarted();
};

#endif /* ACTIONTHREAD_H */
