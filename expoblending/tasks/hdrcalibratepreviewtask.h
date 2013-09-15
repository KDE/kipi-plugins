/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-08-31
 * Description : a plugin to blend bracketed images.
 *
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

#ifndef HDRCALIBRATEPREVIEWTASK_H
#define HDRCALIBRATEPREVIEWTASK_H

// Qt includes

#include <QPointer>

// KDE includes

#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktempdir.h>
// Local includes

#include "task.h"

using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class HdrCalibratePreviewTask : public Task
{
  
  
    Q_OBJECT
  
public:
     
    KUrl::List          urls;
    KTempDir*           preprocessingTmpDir;
    QString const       name;
    
    QProcess*           pfsinhdrgenProcess;
    QProcess*           pfshdrcalibrateProcess;
    QProcess*           pfsoutProcess;
    
    PfsHdrSettings      settings;
    
    KUrl const          destUrl;
   
protected:

    bool         successFlag;
    bool         isAbortedFlag;
    const KUrl   tmpDir;

public:

    HdrCalibratePreviewTask(QObject* const parent, const KUrl::List& inUrls,const QString& dirName,
						 const PfsHdrSettings& pfsSettings,const KUrl& outputUrl);
    HdrCalibratePreviewTask(const KUrl::List& inUrls,const QString& dirName,
						 const PfsHdrSettings& pfsSettings,const KUrl& outputUrl);
    ~HdrCalibratePreviewTask();
    
    bool startpfsHdrCalibrate(const QString& name, QString& errors, 
						   const PfsHdrSettings& settings, 
						   KUrl& exroutput);    

Q_SIGNALS:

    void starting(const KIPIExpoBlendingPlugin::ActionData& ad);
    void finished(const KIPIExpoBlendingPlugin::ActionData& ad);

protected:

    void run();
    
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* HDRCALIBRATEPREVIEWTASK_H */
