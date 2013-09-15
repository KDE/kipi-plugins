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

#include "generictask.moc"
// C++ includes

#include <cmath>

// Qt includes

#include <QFileInfo>
#include <QtDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QPointer>
#include <qdebug.h>

// Under Win32, log2f is not defined...
#ifdef _WIN32
#define log2f(x) (logf(x)*1.4426950408889634f)
#endif

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
  
GenericTask::GenericTask(QObject* const parent, const KUrl::List& fileUrl, const Action& action)
    : Task(parent, action, fileUrl), urls(fileUrl), action(action)
{}

GenericTask::GenericTask(QObject* const parent, const KUrl::List& fileUrl, const Action& action, 
			 EvValueUrls& targetUrls)
    : Task(parent, action, fileUrl), urls(fileUrl), action(action), exposureValuesUrl(&targetUrls)
{}
  
GenericTask::~GenericTask()
{}

void GenericTask::run()
{
    
    switch (action)
    {
        case IDENTIFY:
        {
	    // Identify Exposure.

	    QString avLum;
            float val;
            if (!urls.isEmpty())
            {
                val = getAverageSceneLuminance(urls[0].toLocalFile());
                if (val != -1)
                    avLum.setNum(log2f(val), 'g', 2);
            }
                    
            ActionData ad;
            ad.action  = action;
            ad.inUrls  = urls;
            ad.message = avLum.isEmpty() ? i18n("unknown") : avLum;
            ad.success = avLum.isEmpty();
	
	    exposureValuesUrl->outputUrl = urls[0];
	    exposureValuesUrl->exposure_value = avLum.isEmpty() ? i18n("unknown") : avLum;
	    
	    //qDebug() << exposureValuesUrl->exposure_value;
            emit finished(ad);
	    break;
        }
		
        case LOAD:
        {
		 
            ActionData ad1;
            ad1.action   = LOAD;
            ad1.inUrls   = urls;
            ad1.starting = true;
            emit starting(ad1);

            QImage image;
            bool result  = image.load(urls[0].toLocalFile());

            // rotate image
            if (result)
            {
                KPMetadata meta(urls[0].toLocalFile());
                meta.rotateExifQImage(image, meta.getImageOrientation());
            }

            ActionData ad2;
            ad2.action         = LOAD;
            ad2.inUrls         = urls;
            ad2.success        = result;
            ad2.image          = image;
            emit finished(ad2);
            break;
        }
              
        default:
        {
            qCritical() << "Unknown action specified" << endl;
            break;
        }
    }     
}

QString GenericTask::getProcessError(KProcess* const proc) const
{
    if (!proc) return QString();

    QString std = proc->readAll();
    return (i18n("Cannot run %1:\n\n %2", proc->program()[0], std));
}

/**
 * This function obtains the "average scene luminance" (cd/m^2) from an image file.
 * "average scene luminance" is the L (aka B) value mentioned in [1]
 * You have to take a log2f of the returned value to get an EV value.
 *
 * We are using K=12.07488f and the exif-implied value of N=1/3.125 (see [1]).
 * K=12.07488f is the 1.0592f * 11.4f value in pfscalibration's pfshdrcalibrate.cpp file.
 * Based on [3] we can say that the value can also be 12.5 or even 14.
 * Another reference for APEX is [4] where N is 0.3, closer to the APEX specification of 2^(-7/4)=0.2973.
 *
 * [1] http://en.wikipedia.org/wiki/APEX_system
 * [2] http://en.wikipedia.org/wiki/Exposure_value
 * [3] http://en.wikipedia.org/wiki/Light_meter
 * [4] http://doug.kerr.home.att.net/pumpkin/#APEX
 *
 * This function tries first to obtain the shutter speed from either of
 * two exif tags (there is no standard between camera manifacturers):
 * ExposureTime or ShutterSpeedValue.
 * Same thing for f-number: it can be found in FNumber or in ApertureValue.
 *
 * F-number and shutter speed are mandatory in exif data for EV calculation, iso is not.
 */
float GenericTask::getAverageSceneLuminance(const KUrl& url)
{
    KPMetadata meta;
    meta.load(url.toLocalFile());
    if (!meta.hasExif())
        return -1;

    long num = 1, den = 1;

    // default not valid values

    float    expo = -1.0;
    float    iso  = -1.0;
    float    fnum = -1.0;
    QVariant rationals;

    if (meta.getExifTagRational("Exif.Photo.ExposureTime", num, den))
    {
        if (den)
            expo = (float)(num) / (float)(den);
    }
    else if (getXmpRational("Xmp.exif.ExposureTime", num, den, meta))
    {
        if (den)
            expo = (float)(num) / (float)(den);
    }
    else if (meta.getExifTagRational("Exif.Photo.ShutterSpeedValue", num, den))
    {
        long   nmr = 1, div = 1;
        double tmp = 0.0;

        if (den)
            tmp = exp(log(2.0) * (float)(num) / (float)(den));

        if (tmp > 1.0)
        {
            div = (long)(tmp + 0.5);
        }
        else
        {
            nmr = (long)(1 / tmp + 0.5);
        }

        if (div)
            expo = (float)(nmr) / (float)(div);
    }
    else if (getXmpRational("Xmp.exif.ShutterSpeedValue", num, den, meta))
    {
        long   nmr = 1, div = 1;
        double tmp = 0.0;

        if (den)
            tmp = exp(log(2.0) * (float)(num) / (float)(den));

        if (tmp > 1.0)
        {
            div = (long)(tmp + 0.5);
        }
        else
        {
            nmr = (long)(1 / tmp + 0.5);
        }

        if (div)
            expo = (float)(nmr) / (float)(div);
    }

    kDebug() << url.fileName() << " : expo = " << expo;

    if (meta.getExifTagRational("Exif.Photo.FNumber", num, den))
    {
        if (den)
            fnum = (float)(num) / (float)(den);
    }
    else if (getXmpRational("Xmp.exif.FNumber", num, den, meta))
    {
        if (den)
            fnum = (float)(num) / (float)(den);
    }
    else if (meta.getExifTagRational("Exif.Photo.ApertureValue", num, den))
    {
        if (den)
            fnum = (float)(exp(log(2.0) * (float)(num) / (float)(den) / 2.0));
    }
    else if (getXmpRational("Xmp.exif.ApertureValue", num, den, meta))
    {
        if (den)
            fnum = (float)(exp(log(2.0) * (float)(num) / (float)(den) / 2.0));
    }

    kDebug() << url.fileName() << " : fnum = " << fnum;

    // Some cameras/lens DO print the fnum but with value 0, and this is not allowed for ev computation purposes.

    if (fnum == 0.0)
        return -1.0;

    // If iso is found use that value, otherwise assume a value of iso=100. (again, some cameras do not print iso in exif).

    if (meta.getExifTagRational("Exif.Photo.ISOSpeedRatings", num, den))
    {
        if (den)
            iso = (float)(num) / (float)(den);
    }
    else if (getXmpRational("Xmp.exif.ISOSpeedRatings", num, den, meta))
    {
        if (den)
            iso = (float)(num) / (float)(den);
    }
    else
    {
        iso = 100.0;
    }

    kDebug() << url.fileName() << " : iso = " << iso;

    // At this point the three variables have to be != -1

    if (expo != -1.0 && iso != -1.0 && fnum != -1.0)
    {
        float asl = (expo * iso) / (fnum * fnum * 12.07488f);
        kDebug() << url.fileName() << " : ASL ==> " << asl;

        return asl;
    }

    return -1.0;
}

bool GenericTask::getXmpRational(const char* xmpTagName, long& num, long& den, KPMetadata& meta)
{
    QVariant rationals = meta.getXmpTagVariant(xmpTagName);

    if (!rationals.isNull())
    {
        QVariantList list = rationals.toList();

        if (list.size() == 2)
        {
            num = list[0].toInt();
            den = list[1].toInt();

            return true;
        }
    }
    successFlag = false;
    return false;
}


}  // namespace KIPIExpoBlendingPlugin
