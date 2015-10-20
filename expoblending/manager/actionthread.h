/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

// Local includes

#include "enfusesettings.h"
#include "actions.h"

class QProcess;

namespace KIPI
{
    class MetadataProcessor;
}

using namespace KIPIPlugins;
using namespace KIPI;

namespace KIPIExpoBlendingPlugin
{

class ActionData;

class ActionThread : public QThread
{
    Q_OBJECT

public:

    explicit ActionThread(QObject* const parent);
    ~ActionThread();

    void setEnfuseVersion(const double version);
    void setPreProcessingSettings(bool align);
    void loadProcessed(const QUrl& url);
    void identifyFiles(const QList<QUrl>& urlList);
    void convertRawFiles(const QList<QUrl>& urlList);
    void preProcessFiles(const QList<QUrl>& urlList, const QString& alignPath);
    void enfusePreview(const QList<QUrl>& alignedUrls, const QUrl& outputUrl,
                       const EnfuseSettings& settings, const QString& enfusePath);
    void enfuseFinal(const QList<QUrl>& alignedUrls, const QUrl& outputUrl,
                     const EnfuseSettings& settings, const QString& enfusePath);

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

    void    preProcessingMultithreaded(const QUrl& url, volatile bool& error);
    bool    startPreProcessing(const QList<QUrl>& inUrls, bool  align, const QString& alignPath, QString& errors);
    bool    computePreview(const QUrl& inUrl, QUrl& outUrl);
    bool    convertRaw(const QUrl& inUrl, QUrl& outUrl);

    bool    startEnfuse(const QList<QUrl>& inUrls, QUrl& outUrl,
                        const EnfuseSettings& settings,
                        const QString& enfusePath, QString& errors);

    QString getProcessError(QProcess& proc) const;

    float   getAverageSceneLuminance(const QUrl& url);
    bool    getXmpRational(const QString& xmpTagName, long& num, long& den, MetadataProcessor* const meta);

private:

    class Private;
    Private* const d;
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* ACTIONTHREAD_H */
