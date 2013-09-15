/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-03-15
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

#ifndef TASK_H
#define TASK_H

// KDE includes

#include <threadweaver/Job.h>
#include <kprocess.h>

#include <libkdcraw/kdcraw.h>

//Local includes
#include "actions.h"
#include "kpmetadata.h"

namespace KIPIExpoBlendingPlugin
{

class Task : public ThreadWeaver::Job
{
  
   Q_OBJECT
       
public:

    QString      errString;
    const Action action;

protected:

    bool         		successFlag;
    bool         		isAbortedFlag;
    const KUrl::List   		tmpListDir;
    const KUrl   		tmpDir;
    
public:

    Task(QObject* const parent, Action action, const KUrl::List& workDir);
    Task(QObject* const parent, Action action, const KUrl& workDir);
    ~Task();

    bool success() const;
    void requestAbort();
   
Q_SIGNALS:

    void starting(const KIPIExpoBlendingPlugin::ActionData& ad);
    void finished(const KIPIExpoBlendingPlugin::ActionData& ad);
    void stepFinished(const KIPIExpoBlendingPlugin::ActionData& ad);
    
    void signalStarting(const KIPIExpoBlendingPlugin::ActionData& ad);
    void signalFinished(const KIPIExpoBlendingPlugin::ActionData& ad);


    
protected:

    static QString getProcessError(KProcess& proc);
    
    
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* TASK_H */
