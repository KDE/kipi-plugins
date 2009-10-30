/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// LibKDcraw includes

#include <libkdcraw/rawdecodingsettings.h>

// Local includes

#include "savesettingswidget.h"

namespace KIPIRawConverterPlugin
{

class ActionData;
class ActionThreadPriv;

class ActionThread : public QThread
{
    Q_OBJECT

public:

    ActionThread(QObject* parent, bool updateFileTimeStamp=false);
    ~ActionThread();

    void setRawDecodingSettings(KDcrawIface::RawDecodingSettings rawDecodingSettings, 
                                SaveSettingsWidget::OutputFormat outputFormat);

    void identifyRawFile(const KUrl& url, bool full=false);
    void identifyRawFiles(const KUrl::List& urlList, bool full=false);

    void thumbRawFile(const KUrl& url);
    void thumbRawFiles(const KUrl::List& urlList);

    void processHalfRawFile(const KUrl& url);
    void processHalfRawFiles(const KUrl::List& urlList);

    void processRawFile(const KUrl& url);
    void processRawFiles(const KUrl::List& urlList);

    void cancel();

Q_SIGNALS:

    void starting(const KIPIRawConverterPlugin::ActionData& ad);
    void finished(const KIPIRawConverterPlugin::ActionData& ad);

protected:

    void run();

private:

    ActionThreadPriv* const d;
};

}  // namespace KIPIRawConverterPlugin

#endif /* ACTIONTHREAD_H */
