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

public:

    int                         id;
   
    ItemPreprocessedUrls* const preProcessedUrl;
    const KUrl                  fileUrl;
    const RawDecodingSettings   settings;
   
    KUrl::List          	urls;
    QString             	binaryPath;
    bool 			align;
   
    KProcess*           	enfuseProcess;
    KProcess*           	alignProcess;
    
    QPointer<KDcraw>            rawProcess;
    

public:

    PreProcessTask(QObject* const parent, const KUrl& workDir, int id, ItemPreprocessedUrls& targetUrls,
                               const KUrl& sourceUrl, const RawDecodingSettings& rawSettings, 
			       const KUrl::List& fileUrl,const QString& alignPath, const bool align);
    PreProcessTask(const KUrl& workDir, int id, ItemPreprocessedUrls& targetUrls,
                               const KUrl& sourceUrl, const RawDecodingSettings& rawSettings, 
			       const KUrl::List& fileUrl,const QString& alignPath, const bool align);
    ~PreProcessTask();

    void requestAbort();

protected:

    void run();

private:

    bool enfuseAlign(const KUrl::List& inUrls, ItemUrlsMap& preProcessedUrlsMap,
                                        bool align, const QString& alignPath, QString& errors);
    bool computePreview(const KUrl& inUrl);
    bool computePreview(const KUrl& inUrl, KUrl& outUrl, KTempDir* preprocessingTmpDir);
    bool convertRaw();
    QString getProcessError(KProcess* const proc) const;
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* PREPROCESSTASK_H */
