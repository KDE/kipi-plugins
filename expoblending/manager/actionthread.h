/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2013 by Soumajyoti Sarkar <ergy dot ergy at gmail dot com>
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

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <threadweaver/DependencyPolicy.h>



// LibKDcraw includes

#include <libkdcraw/rawdecodingsettings.h>
#include <libkdcraw/ractionthreadbase.h>

// Local includes

#include "enfusesettings.h"
#include "actions.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class ActionData;

class ActionThread : public RActionThreadBase
{
    Q_OBJECT

public:

    explicit ActionThread(QObject* const parent);
    //~ActionThread();
    
    void setPreProcessingSettings(bool align, const RawDecodingSettings& settings);
    void setEnfuseVersion(const double version);
    void startPreProcessing(const KUrl::List& inUrls,
                                      bool align, const RawDecodingSettings& rawSettings,
                                      const QString& alignPath);
    void loadProcessed(const KUrl& url);
    void identifyFiles(const KUrl::List& urlList);
    void preProcessFiles(const KUrl::List& urlList, const QString& alignPath); 
    void enfusePreview(const KUrl::List& alignedUrls, const KUrl& outputUrl,
                       const EnfuseSettings& settings, const QString& enfusePath);
    void enfuseFinal(const KUrl::List& alignedUrls, const KUrl& outputUrl,
                     const EnfuseSettings& settings, const QString& enfusePath);
    void startPreProcessing(const KUrl::List& inUrls, ItemUrlsMap& preProcessedMap,
                                      bool align, const RawDecodingSettings& rawSettings,
                                      const QString& alignPath, QString& errors);


    void cancel();

    /**
     * Clean up all temporary results produced so far.
     */
    void cleanUpResultFiles();

Q_SIGNALS:

    void starting(const KIPIExpoBlendingPlugin::ActionData& ad);
    void stepFinished(const KIPIExpoBlendingPlugin::ActionData& ad);
    void finished(const KIPIExpoBlendingPlugin::ActionData& ad);
    
private Q_SLOTS:

    void slotDone(ThreadWeaver::Job* j);
    void slotStepDone(ThreadWeaver::Job* j);
    void slotStarting(ThreadWeaver::Job* j);
  
private:

    class ActionThreadPriv;
    ActionThreadPriv* const d;
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* ACTIONTHREAD_H */
