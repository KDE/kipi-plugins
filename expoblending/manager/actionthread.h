/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <kprocess.h>

// LibKexiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "savesettingswidget.h"
#include "enfusesettings.h"
#include "actions.h"

using namespace KIPIPlugins;
using namespace KExiv2Iface;

namespace KIPIExpoBlendingPlugin
{

class ActionData;
class ActionThreadPriv;

class ActionThread : public QThread
{
    Q_OBJECT

public:

    explicit ActionThread(QObject* parent);
    ~ActionThread();

    void setSettings(const EnfuseSettings& enfuseSettings, SaveSettingsWidget::OutputFormat frmt);
    void identifyFiles(const KUrl::List& urlList);
    void convertRawFiles(const KUrl::List& urlList);
    void alignFiles(const KUrl::List& urlList);
    void enfuseFiles(const KUrl::List& alignedUrls, const KUrl& outputUrl);
    void convertRawFiles(const KUrl::List& alignedUrls, const KUrl& outputUrl);

    void cancel();

Q_SIGNALS:

    void starting(const KIPIExpoBlendingPlugin::ActionData& ad);
    void finished(const KIPIExpoBlendingPlugin::ActionData& ad);

private:

    void    run();

    bool    startAlign(const KUrl::List& inUrls, ItemUrlsMap& alignedUrlsMap,
                       QString& errors);

    bool    startEnfuse(const KUrl::List& inUrls, KUrl& outUrl,
                        const EnfuseSettings& enfuseSettings,
                        SaveSettingsWidget::OutputFormat frmt,
                        QString& errors);

    bool    startConvertRaw(const KUrl::List& inUrls, KUrl::List& outUrls);
    bool    isRAWFile(const KUrl& url);

    QString getProcessError(KProcess* proc) const;

    float   getAverageSceneLuminance(const KUrl& url);
    bool    getXmpRational(const char* xmpTagName, long& num, long& den, KExiv2& meta);

private:

    ActionThreadPriv* const d;
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* ACTIONTHREAD_H */
