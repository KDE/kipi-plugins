/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-24
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2008-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QThread>

// KDE includes

#include <kurl.h>

// Local includes

#include "settingswidget.h"

namespace KIPIDNGConverterPlugin
{

class ActionData;

class ActionThread : public QThread
{
    Q_OBJECT

public:

    ActionThread(QObject* parent);
    ~ActionThread();

    void setBackupOriginalRawFile(bool b);
    void setCompressLossLess(bool b);
    void setUpdateFileDate(bool b);
    void setPreviewMode(int mode);

    void identifyRawFile(const KUrl& url, bool full=false);
    void identifyRawFiles(const KUrl::List& urlList, bool full=false);

    void thumbRawFile(const KUrl& url);
    void thumbRawFiles(const KUrl::List& urlList);

    void processRawFile(const KUrl& url);
    void processRawFiles(const KUrl::List& urlList);

    void cancel();

Q_SIGNALS:

    void starting(const KIPIDNGConverterPlugin::ActionData& ad);
    void finished(const KIPIDNGConverterPlugin::ActionData& ad);

protected:

    void run();

private:

    class ActionThreadPriv;
    ActionThreadPriv* const d;
};

}  // namespace KIPIDNGConverterPlugin

#endif /* ACTIONTHREAD_H */
