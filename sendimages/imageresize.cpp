/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-09
 * Description : a class to resize image in a separate thread.
 *
 * Copyright (C) 2007-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "imageresize.moc"

// Qt includes

#include <QDir>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ktempdir.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "kpversion.h"
#include "kpwriteimage.h"

using namespace ThreadWeaver;

namespace KIPISendimagesPlugin
{

class ImageResize::Task : public Job
{
public:

    Task(QObject* parent = 0, int *count = 0)
            :Job(parent)
    {
        this->count = count;
    }

    KUrl                   orgUrl;
    QString                destName;
    EmailSettingsContainer settings;

    int            *count;

private:

    bool imageResize(const EmailSettingsContainer& settings,
                     const KUrl& orgUrl, const QString& destName, QString& err);

Q_SIGNALS:

    void startingResize(const KUrl &orgUrl) {}
    void finishedResize(const KUrl &orgUrl, const KUrl& emailUrl, int percent) {}
    void failedResize(const KUrl &orgUrl, const QString &errString, int percent) {}
    void completeResize() {}

protected:

    void run()
    {
        QString errString;

        emit startingResize(orgUrl);
        (*count)++;
        int percent = (int)(((float)(*count)/(float)settings.itemsList.count())*100.0);

        if (imageResize(settings, orgUrl, destName, errString))
        {
            KUrl emailUrl(destName);
            emit finishedResize(orgUrl, emailUrl, percent);
        }
        else
        {
            emit failedResize(orgUrl, errString, percent);
        }

        if (settings.itemsList.count() == *count)
        {
            emit completeResize();
            *count = 0;
        }
    }
};

bool ImageResize::Task::imageResize(const KIPISendimagesPlugin::EmailSettingsContainer& settings, const KUrl& orgUrl, const QString& destName, QString& err)
{
    EmailSettingsContainer emailSettings = settings;
    QFileInfo fi(orgUrl.path());

    if (!fi.exists() || !fi.isReadable())
    {
        err = i18n("Error opening input file");
        return false;
    }

    QFileInfo tmp(destName);
    QFileInfo tmpDir(tmp.dir().absolutePath());

    kDebug() << "tmpDir: " << tmp.dir().absolutePath();

    if (!tmpDir.exists() || !tmpDir.isWritable())
    {
        err = i18n("Error opening temporary folder");
        return false;
    }

    QImage img;

    // Check if RAW file.
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
    if (rawFilesExt.toUpper().contains( fi.suffix().toUpper() ))
        KDcrawIface::KDcraw::loadDcrawPreview(img, orgUrl.path());
    else
        img.load(orgUrl.path());

    int sizeFactor = emailSettings.size();

    if ( !img.isNull() )
    {
        int w = img.width();
        int h = img.height();

        if ( w > sizeFactor || h > sizeFactor )
        {
            if ( w > h )
            {
                h = (int)( (double)( h * sizeFactor ) / w );

                if ( h == 0 ) h = 1;

                w = sizeFactor;
                Q_ASSERT( h <= sizeFactor );
            }
            else
            {
                w = (int)( (double)( w * sizeFactor ) / h );

                if ( w == 0 ) w = 1;

                h = sizeFactor;
                Q_ASSERT( w <= sizeFactor );
            }

            const QImage scaledImg(img.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

            if ( scaledImg.width() != w || scaledImg.height() != h )
            {
                err = i18n("Cannot resize image. Aborting.");
                return false;
            }

            img = scaledImg;
        }

        QString destPath = destName;

        KExiv2Iface::KExiv2 meta;

        meta.load(orgUrl.path());
        meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
        meta.setImageDimensions(img.size());

        if (emailSettings.format() == QString("JPEG"))
        {
            if ( !img.save(destPath, emailSettings.format().toLatin1(), emailSettings.imageCompression) )
            {
                err = i18n("Cannot save resized image (JPEG). Aborting.");
                return false;
            }
            else
            {
                meta.save(destPath);
            }
        }
        else if (emailSettings.format() == QString("PNG"))
        {
            QByteArray data((const char*)img.bits(), img.numBytes());
            KIPIPlugins::KPWriteImage wImageIface;
            wImageIface.setImageData(data, img.width(), img.height(), false, true, QByteArray(), meta);
            if ( !wImageIface.write2PNG(destPath) )
            {
                err = i18n("Cannot save resized image (PNG). Aborting.");
                return false;
            }
        }

        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------------------------------

ImageResize::ImageResize(QObject *parent)
        : ActionThreadBase(parent)
{
    count = new int;
    *count = 0;
}

ImageResize::~ImageResize()
{
    delete count;
}

void ImageResize::resize(const EmailSettingsContainer& settings)
{
    JobCollection* collection = new JobCollection(this);
    *count = 0;
    int i    = 1;

    for (QList<EmailItem>::const_iterator it = settings.itemsList.constBegin();
            it != settings.itemsList.constEnd(); ++it)
    {
        QString tmp;

        Task *t = new Task(this,count);
        t->orgUrl                = (*it).orgUrl;
        t->settings              = settings;

        KTempDir tmpDir(KStandardDirs::locateLocal("tmp", t->settings.tempFolderName + t->settings.tempPath), 0700);
        tmpDir.setAutoRemove(false);
        QFileInfo fi(t->orgUrl.fileName());
        t->destName = tmpDir.name() + QString("%1.%2").arg(fi.baseName()).arg(t->settings.format().toLower());

        connect(t,SIGNAL(startingResize(KUrl)),this,SIGNAL(startingResize(KUrl)));
        connect(t,SIGNAL(finishedResize(KUrl,KUrl,int)),this,SIGNAL(finishedResize(KUrl,KUrl,int)));
        connect(t,SIGNAL(failedResize(KUrl,QString,int)),this,SIGNAL(failedResize(KUrl,QString,int)));
        connect(t,SIGNAL(completeResize()),this,SIGNAL(completeResize()));

        collection->addJob(t);
        i++;
    }

    appendJob(collection);
}

void ImageResize::cancel()
{
    *count   = 0;
    ActionThreadBase::cancel();
}

}  // namespace KIPISendimagesPlugin
