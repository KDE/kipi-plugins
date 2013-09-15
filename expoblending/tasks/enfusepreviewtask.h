/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-06-28
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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


#ifndef ENFUSEPREVIEWTASK_H
#define ENFUSEPREVIEWTASK_H

// KDE includes

#include <threadweaver/Job.h>
#include <kprocess.h>
#include <ktempdir.h>


#include "task.h"

#include <libkdcraw/kdcraw.h>
#include <libkdcraw/rawdecodingsettings.h>

//Local includes
#include "enfusesettings.h"
#include "actions.h"
#include "kpmetadata.h"


using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class EnfusePreviewTask : public Task
{
  
   Q_OBJECT
   
    
public:

    KUrl::List          		urls;
    KUrl                		outputUrl;
    EnfuseSettings      		enfuseSettings;
    QString             		binaryPath;
    bool 				enfuseVersion4x;
    
    KProcess*           		enfuseProcess;
    
    KTempDir*                        	preprocessingTmpDir;

protected:

    bool         successFlag;
    bool         isAbortedFlag;
    const KUrl   tmpDir;

public:
    EnfusePreviewTask(QObject* const parent, const KUrl::List& fileUrl, const KUrl& outputUrl,
			         const EnfuseSettings& settings, const QString& alignPath, bool version);
    EnfusePreviewTask(const KUrl::List& fileUrl, const KUrl& outputUrl,
			         const EnfuseSettings& settings, const QString& alignPath, bool version);

    ~EnfusePreviewTask();

    void setEnfuseVersion(const double version);
    bool startEnfuse(const KUrl::List& inUrls, KUrl& outUrl,
                               const EnfuseSettings& settings,
                               const QString& enfusePath, QString& errors);
    

Q_SIGNALS:

    void starting(const KIPIExpoBlendingPlugin::ActionData& ad);
    void finished(const KIPIExpoBlendingPlugin::ActionData& ad);


protected:

    void run() ;
    QString getProcessError(KProcess* const proc) const;
        
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* ENFUSEPREVIEWTASK_H */
