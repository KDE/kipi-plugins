/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
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

#include "actions.h"
#include "savesettingswidget.h"

using namespace KIPIPlugins;
using namespace KExiv2Iface;
using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

struct ActionData;

class ActionThread : public QThread
{
    Q_OBJECT

public:

    explicit ActionThread(QObject* parent);
    ~ActionThread();

    void setPreProcessingSettings(bool celeste, const KDcrawIface::RawDecodingSettings& settings);
    void convertRawFiles(const KUrl::List& urlList);
    void preProcessFiles(const KUrl::List& urlList);
    void optimizeProject(const KUrl& ptoUrl);
    void cancel();

    /**
     * Clean up all temporary results produced so far.
     */
    void cleanUpResultFiles();

Q_SIGNALS:

    void starting(const KIPIPanoramaPlugin::ActionData& ad);
    void stepFinished();
    void finished(const KIPIPanoramaPlugin::ActionData& ad);

private:

    void    run();

    bool    startPreProcessing(const KUrl::List& inUrls, ItemUrlsMap& preProcessedUrlsMap,
                               const RawDecodingSettings& settings);
    bool    startCPFind(KUrl& ptoUrl, ItemUrlsMap& preProcessedUrlsMap, bool celeste, QString& errors);
    bool    startOptimization(KUrl& ptoUrl, QString& errors);
    bool    computePreview(const KUrl& inUrl, KUrl& outUrl);
    bool    convertRaw(const KUrl& inUrl, KUrl& outUrl, const RawDecodingSettings& settings);
    bool    isRawFile(const KUrl& url);
    bool    createPTO(const KIPIPanoramaPlugin::ItemUrlsMap& urlList, KUrl& ptoUrl);

    QString getProcessError(KProcess* proc) const;

private:

    struct ActionThreadPriv;
    ActionThreadPriv* const d;
};

}  // namespace KIPIPanoramaPlugin

#endif /* ACTIONTHREAD_H */
