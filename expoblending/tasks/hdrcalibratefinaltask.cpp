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

#include "hdrcalibratefinaltask.moc"

// Qt includes

#include <QFileInfo>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>

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

HdrCalibrateFinalTask::HdrCalibrateFinalTask(QObject* const parent, const KUrl::List& inUrls,const QString& dirName,
				   const PfsHdrSettings& pfsSettings, const KUrl& outputUrl)
    : Task(parent, HDRCALIBRATEFINAL, inUrls), urls(inUrls), name(dirName), settings(pfsSettings), destUrl(outputUrl)
{}

HdrCalibrateFinalTask::HdrCalibrateFinalTask(const KUrl::List& inUrls, const QString& dirName,
				   const PfsHdrSettings& pfsSettings, const KUrl& outputUrl)
    : Task(0, HDRCALIBRATEFINAL, inUrls), urls(inUrls), name(dirName), settings(pfsSettings), destUrl(outputUrl)
{}

HdrCalibrateFinalTask::~HdrCalibrateFinalTask()
{}

void HdrCalibrateFinalTask::run()
{   
    ActionData ad1;
    ad1.action         = HDRCALIBRATEFINAL;
    ad1.inUrls         = urls;
    ad1.pfshdrSettings = settings;
    ad1.starting       = true;
    emit starting(ad1);
    
    QString errors;
    KUrl outUrl = destUrl;
    bool result = startpfsHdrCalibrate(name, errors, settings, outUrl);
 
    // We will take first image metadata from stack to restore Exif, Iptc, and Xmp.
    KPMetadata meta;
    meta.load(urls[0].toLocalFile());
    result = result & meta.setXmpTagString("Xmp.kipi.PfsHdrInputFiles", settings.inputImagesList(), false);
    meta.setImageDateTime(QDateTime::currentDateTime());
    if (settings.outputFormat != KPSaveSettingsWidget::OUTPUT_JPEG)
    {
	QImage img;
        if (img.load(outUrl.toLocalFile()))
            meta.setImagePreview(img.scaled(1280, 1024, Qt::KeepAspectRatio));
    }
    meta.save(outUrl.toLocalFile());  
    
    ActionData ad2;
    ad2.action         = HDRCALIBRATEFINAL;
    ad2.inUrls         = urls;
    ad2.outUrls        = KUrl::List() << outUrl;
    ad2.success        = result;
    ad2.message        = errors;
    ad2.pfshdrSettings = settings;
    
    emit finished(ad2); 
    
    return ; 
}

bool HdrCalibrateFinalTask::startpfsHdrCalibrate(const QString& name, QString& errors, 
						 const PfsHdrSettings& settings, 
						 KUrl& exroutput)
{
    QString ext = KPSaveSettingsWidget::extensionForFormat(settings.outputFormat);
    exroutput.setFileName(QString(".kipi-hdr-exiftags-tmp-") + QString::number(QDateTime::currentDateTime().toTime_t()) + ext);
    
    KUrl exifTags = KUrl(name + QString("exifTags.hdrgen"));
    KUrl cameraResponse= KUrl(name + QString("camera.response"));
    
    QStringList pfsinhdrgenArgs;
    QStringList pfshdrcalibrateArgs;
    QStringList pfsoutArgs;
    
    pfsinhdrgenProcess = new QProcess();
    pfshdrcalibrateProcess = new QProcess();
    pfsoutProcess = new QProcess();
    
    pfsinhdrgenArgs << exifTags.toLocalFile();
    
    pfshdrcalibrateArgs << "-v";
    pfshdrcalibrateArgs << "-f";
    pfshdrcalibrateArgs << cameraResponse.toLocalFile();
    
    pfsoutArgs << exroutput.toLocalFile();
    
    pfsinhdrgenProcess->setWorkingDirectory(name);
    pfshdrcalibrateProcess->setWorkingDirectory(name);
    
    pfsinhdrgenProcess->setStandardOutputProcess(pfshdrcalibrateProcess); 
    pfshdrcalibrateProcess->setStandardOutputProcess(pfsoutProcess);

    pfsinhdrgenProcess->start("pfsinhdrgen", pfsinhdrgenArgs);
    pfshdrcalibrateProcess->start("pfshdrcalibrate", pfshdrcalibrateArgs); 
    pfsoutProcess->start("pfsout", pfsoutArgs);

    if (!pfsoutProcess->waitForFinished(-1))
    {
        successFlag = false;  
        //errors = getProcessError(pfsoutexrProcess);
        return 0;
    }
    pfsinhdrgenProcess->close();
    pfshdrcalibrateProcess->close();
    pfsoutProcess->close();
    
    return 1;
}

/*QString HdrCalibrateTask::getProcessError(QProcess* const proc) const
{
    if (!proc) return QString();

    QString std = proc->readAll();
    return (i18n("Cannot run %1:\n\n %2", proc->program()[0], std));
}*/

} // namespace KIPIExpoBlendingPlugin