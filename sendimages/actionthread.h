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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes.

#include <QThread>
#include <QStringList>

// KDE includes.

#include <kurl.h>

// Local includes.

#include "emailsettingscontainer.h"

namespace KIPISendimagesPlugin
{

class ActionThreadPriv;

class ActionThread : public QThread
{
    Q_OBJECT

public:

    ActionThread(QObject *parent);
    ~ActionThread();

    void resize(const EmailSettingsContainer& settings);
    void cancel();

protected:

    void run();

signals:

    void startingResize(const KUrl &fileUrl);
    void finishedResize(const KUrl &fileUrl, const QString& resizedImgPath);
    void failedResize(const KUrl &fileUrl, const QString &errString);
    void completeResize();

private:

    ActionThreadPriv *d;
};

}  // NameSpace KIPISendimagesPlugin

#endif /* ACTIONTHREAD_H */
