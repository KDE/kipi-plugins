/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
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

#include <QUrl>

// LibKDcraw includes

#include <rawdecodingsettings.h>
#include <ractionthreadbase.h>

// Local includes

#include "kpsavesettingswidget.h"
#include "actions.h"

using namespace KDcrawIface;
using namespace ThreadWeaver;
using namespace KIPIPlugins;

namespace KIPIRawConverterPlugin
{

class ActionThread : public RActionThreadBase
{
    Q_OBJECT

public:

    explicit ActionThread(QObject* const parent);
    ~ActionThread();

    void setSettings(const RawDecodingSettings& rawDecodingSettings,
                     KPSaveSettingsWidget::OutputFormat outputFormat);

    void identifyRawFile(const QUrl& url, bool full=false);
    void identifyRawFiles(const QUrl::List& urlList, bool full=false);

    void thumbRawFile(const QUrl& url);
    void thumbRawFiles(const QUrl::List& urlList);

    void processHalfRawFile(const QUrl& url);
    void processHalfRawFiles(const QUrl::List& urlList);

    void processRawFile(const QUrl& url);
    void processRawFiles(const QUrl::List& urlList);

    void cancel();

Q_SIGNALS:

    void signalStarting(const KIPIRawConverterPlugin::ActionData& ad);
    void signalFinished(const KIPIRawConverterPlugin::ActionData& ad);

    /** Signal to emit to sub-tasks to cancel processing.
     */
    void signalCancelTask();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPIRawConverterPlugin

#endif /* ACTIONTHREAD_H */
