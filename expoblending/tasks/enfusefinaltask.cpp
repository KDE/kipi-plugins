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

#include "enfusefinaltask.moc"
// C++ includes

#include <cmath>

// Qt includes

#include <QFileInfo>
#include <QtDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QPointer>

// KDE includes
#include <klocale.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <ktempdir.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

//Local includes
#include "actions.h"
#include "kpmetadata.h"
#include "kpwriteimage.h"
#include "kpversion.h"

namespace KIPIExpoBlendingPlugin
{
  
EnfuseFinalTask::EnfuseFinalTask(QObject* const parent, const KUrl::List& fileUrl, const KUrl& outputUrl, 
			 const EnfuseSettings& settings, const QString& alignPath, bool version)
    : Task(parent, ENFUSEFINAL, fileUrl), urls(fileUrl), outputUrl(outputUrl), enfuseSettings(settings), 
	   binaryPath(alignPath), enfuseVersion4x(version)
{}

EnfuseFinalTask::EnfuseFinalTask(const KUrl::List& fileUrl, const KUrl& outputUrl, 
			 const EnfuseSettings& settings, const QString& alignPath, bool version)
    : Task(0, ENFUSEFINAL, fileUrl), urls(fileUrl), outputUrl(outputUrl), enfuseSettings(settings), 
	   binaryPath(alignPath), enfuseVersion4x(version)
{}

EnfuseFinalTask::~EnfuseFinalTask()
{}


void EnfuseFinalTask::run()
{
                
    ActionData ad1;
    ad1.action         = ENFUSEFINAL;
    ad1.inUrls         = urls;
    ad1.starting       = true;
    ad1.enfuseSettings = enfuseSettings;
    emit starting(ad1);

    KUrl    destUrl = outputUrl;
    bool    result  = false;
    QString errors;

    result = startEnfuse(urls, destUrl, enfuseSettings, binaryPath, errors);

    // We will take first image metadata from stack to restore Exif, Iptc, and Xmp.
    KPMetadata meta;
    meta.load(urls[0].toLocalFile());
    result = result & meta.setXmpTagString("Xmp.kipi.EnfuseInputFiles", enfuseSettings.inputImagesList(), false);
    result = result & meta.setXmpTagString("Xmp.kipi.EnfuseSettings", enfuseSettings.asCommentString().replace('\n', " ; "), false);
    meta.setImageDateTime(QDateTime::currentDateTime());
    if (enfuseSettings.outputFormat != KPSaveSettingsWidget::OUTPUT_JPEG)
    {
	QImage img;
        if (img.load(destUrl.toLocalFile()))
        meta.setImagePreview(img.scaled(1280, 1024, Qt::KeepAspectRatio));
    }
    meta.save(destUrl.toLocalFile());                

    ActionData ad2;
    ad2.action         = ENFUSEFINAL;
    ad2.inUrls         = urls;
    ad2.outUrls        = KUrl::List() << destUrl;
    ad2.success        = result;
    ad2.message        = errors;
    ad2.enfuseSettings = enfuseSettings;
    emit finished(ad2); 
                    
}

bool EnfuseFinalTask::startEnfuse(const KUrl::List& inUrls, KUrl& outUrl,
                               const EnfuseSettings& settings,
                               const QString& enfusePath, QString& errors)
{
    QString comp;
    QString ext = KPSaveSettingsWidget::extensionForFormat(settings.outputFormat);

    if (ext == QString(".tif"))
        comp = QString("--compression=DEFLATE");

    outUrl.setFileName(QString(".kipi-expoblending-tmp-") + QString::number(QDateTime::currentDateTime().toTime_t()) + ext);

    enfuseProcess = new KProcess;
    enfuseProcess->clearProgram();
    enfuseProcess->setOutputChannelMode(KProcess::MergedChannels);
    QStringList args;
    args << enfusePath;

    if (!settings.autoLevels)
    {
        args << "-l";
        args << QString::number(settings.levels);
    }

    if (settings.ciecam02)
        args << "-c";

    if (!comp.isEmpty())
        args << comp;

    if (settings.hardMask)
    {
        if (enfuseVersion4x)
            args << "--hard-mask";
        else
            args << "--HardMask";
    }

    if (enfuseVersion4x)
    {
        args << QString("--exposure-weight=%1").arg(settings.exposure);
        args << QString("--saturation-weight=%1").arg(settings.saturation);
        args << QString("--contrast-weight=%1").arg(settings.contrast);
    }
    else
    {
        args << QString("--wExposure=%1").arg(settings.exposure);
        args << QString("--wSaturation=%1").arg(settings.saturation);
        args << QString("--wContrast=%1").arg(settings.contrast);
    }

    args << "-v";
    args << "-o";
    args << outUrl.toLocalFile();

    foreach(const KUrl& url, inUrls)
        args << url.toLocalFile();

    enfuseProcess->setProgram(args);

    kDebug() << "Enfuse command line: " << enfuseProcess->program();

    enfuseProcess->start();

    if (!enfuseProcess->waitForFinished(-1))
    {
        successFlag = false;
        errors = getProcessError(enfuseProcess);
        return false;
    }

    kDebug() << "Enfuse output url: "  << outUrl;
    kDebug() << "Enfuse exit status: " << enfuseProcess->exitStatus();
    kDebug() << "Enfuse exit code:   " << enfuseProcess->exitCode();

    if (enfuseProcess->exitStatus() != QProcess::NormalExit)
    {
        successFlag = false;
        return false;
    }

    if (enfuseProcess->exitCode() == 0)
    {
        // Process finished successfully !
        return true;
    }

    errors = getProcessError(enfuseProcess);
    successFlag = false;
    return false;
}

QString EnfuseFinalTask::getProcessError(KProcess* const proc) const
{
    if (!proc) return QString();

    QString std = proc->readAll();
    return (i18n("Cannot run %1:\n\n %2", proc->program()[0], std));
}

}  // namespace KIPIExpoBlendingPlugin
