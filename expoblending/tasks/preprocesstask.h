/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
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

#ifndef PREPROCESSTASK_H
#define PREPROCESSTASK_H

// Qt includes

#include <QPointer>

// KDE includes

#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktempdir.h>


#include <threadweaver/Job.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "task.h"

using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class PreProcessTask : public Task
{
    Q_OBJECT
         
public:
   
    KUrl::List          		urls;
    const RawDecodingSettings   	settings;
   
    bool 				align;
    QString             		binaryPath;
    bool 				cancel;
    
    KProcess*           		enfuseProcess;
    KProcess*           		alignProcess;
    
    KTempDir*                        	preprocessingTmpDir;
   
protected:

    bool         successFlag;
    bool         isAbortedFlag;
    const KUrl   tmpDir;

public:

    PreProcessTask(QObject* const parent, const KUrl::List& inUrls, 
		   const RawDecodingSettings& rawSettings, const bool align,
	           const QString& alignPath);
    PreProcessTask(const KUrl::List& inUrls, const RawDecodingSettings& rawSettings, 
		   const bool align, const QString& alignPath);   
    ~PreProcessTask();
    
    bool startPreProcessing(const KUrl::List& inUrls, ItemUrlsMap& preProcessedUrlsMap,
                                      bool align, const RawDecodingSettings& settings,
                                      const QString& alignPath, QString& errors);
    void cleanAlignTmpDir();
    bool computePreview(const KUrl& inUrl, KUrl& outUrl);
    bool convertRaw(const KUrl& inUrl, KUrl& outUrl, const RawDecodingSettings& settings);
    QString getProcessError(KProcess* const proc) const;

Q_SIGNALS:

    void starting(const KIPIExpoBlendingPlugin::ActionData& ad);
    void finished(const KIPIExpoBlendingPlugin::ActionData& ad);

protected:

    void run();
    
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* PREPROCESSTASK_H */
