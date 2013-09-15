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

#include "cameraresponsetask.h"

// Qt includes

#include <QFileInfo>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>

// KDE includes

#include <klocale.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <ktempdir.h>
// Local includes

#include "kpmetadata.h"
#include "kpversion.h"
#include "kpwriteimage.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

CameraResponseTask::CameraResponseTask(QObject* const parent, const KUrl::List& inUrls, const QString& dirName, 
				       const PfsHdrSettings& pfsSettings, int option)
    : Task(parent, CAMERARESPONSE, inUrls), name(dirName), settings(pfsSettings), option(option)
{}

CameraResponseTask::CameraResponseTask(const KUrl::List& inUrls, const QString& dirName, 
				       const PfsHdrSettings& pfsSettings, int option)
    : Task(0, CAMERARESPONSE, inUrls), name(dirName), settings(pfsSettings), option(option)
{}

CameraResponseTask::~CameraResponseTask()
{}

void CameraResponseTask::run()
{   
    ActionData ad1;
    ad1.action         = CAMERARESPONSE;
    ad1.starting       = true;
    emit starting(ad1);
    
    KUrl exifTags = KUrl(name + QString("exifTags.hdrgen"));
    KUrl cameraResponse= KUrl(name + QString("camera.response"));
    
    QStringList pfsinhdrgenArgs;
    QStringList pfshdrcalibrateArgs;
    
    pfsinhdrgenProcess = new QProcess();
    pfshdrcalibrateProcess = new QProcess();
    
    pfsinhdrgenArgs << exifTags.toLocalFile();
    
    pfshdrcalibrateArgs << "-v";
    pfshdrcalibrateArgs << "-s";
    pfshdrcalibrateArgs << cameraResponse.toLocalFile();
    
    pfsinhdrgenProcess->setWorkingDirectory(name);
    pfshdrcalibrateProcess->setWorkingDirectory(name);
    
    pfsinhdrgenProcess->setStandardOutputProcess(pfshdrcalibrateProcess); 

    pfsinhdrgenProcess->start("pfsinhdrgen", pfsinhdrgenArgs);
    pfshdrcalibrateProcess->start("pfshdrcalibrate", pfshdrcalibrateArgs); 

    if (!pfshdrcalibrateProcess->waitForFinished(-1))
    {
        successFlag = false;  
        //errors = getProcessError(pfsoutexrProcess);
        return ;
    }
    pfsinhdrgenProcess->close();
    pfshdrcalibrateProcess->close();
    
    ActionData ad2;
    ad2.action         = CAMERARESPONSE;
    ad2.option         = option;
    ad2.dirName        = name;
    ad2.pfshdrSettings = settings;
    
    emit finished(ad2);
                    
}

/*QString HdrCalibrateTask::getProcessError(QProcess* const proc) const
{
    if (!proc) return QString();

    QString std = proc->readAll();
    return (i18n("Cannot run %1:\n\n %2", proc->program()[0], std));
}*/

} // namespace KIPIExpoBlendingPlugin