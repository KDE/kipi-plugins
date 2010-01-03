/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
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

// LibKDcraw includes

#include <libkdcraw/rawdecodingsettings.h>

// Local includes

#include "enfusesettings.h"
#include "actions.h"

using namespace KIPIPlugins;
using namespace KExiv2Iface;
using namespace KDcrawIface;

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

    void setEnfuseVersion(const QString& version);
    void setPreProcessingSettings(bool align, const RawDecodingSettings& settings);
    void loadProcessed(const KUrl& url);
    void identifyFiles(const KUrl::List& urlList);
    void convertRawFiles(const KUrl::List& urlList);
    void preProcessFiles(const KUrl::List& urlList);
    void enfusePreview(const KUrl::List& alignedUrls, const KUrl& outputUrl, const EnfuseSettings& settings);
    void enfuseFinal(const KUrl::List& alignedUrls, const KUrl& outputUrl, const EnfuseSettings& settings);

    void cancel();

    /**
     * Clean up all temporary results produced so far.
     */
    void cleanUpResultFiles();

Q_SIGNALS:

    void starting(const KIPIExpoBlendingPlugin::ActionData& ad);
    void finished(const KIPIExpoBlendingPlugin::ActionData& ad);

private:

    void    run();

    bool    startPreProcessing(const KUrl::List& inUrls, ItemUrlsMap& preProcessedUrlsMap,
                               bool align, const RawDecodingSettings& settings,
                               QString& errors);
    bool    computePreview(const KUrl& inUrl, KUrl& outUrl);
    bool    convertRaw(const KUrl& inUrl, KUrl& outUrl, const RawDecodingSettings& settings);
    bool    isRawFile(const KUrl& url);

    bool    startEnfuse(const KUrl::List& inUrls, KUrl& outUrl,
                        const EnfuseSettings& settings,
                        QString& errors);

    QString getProcessError(KProcess* proc) const;

    float   getAverageSceneLuminance(const KUrl& url);
    bool    getXmpRational(const char* xmpTagName, long& num, long& den, KExiv2& meta);

private:

    ActionThreadPriv* const d;
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* ACTIONTHREAD_H */
