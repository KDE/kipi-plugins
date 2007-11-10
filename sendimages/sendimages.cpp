/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-02-25
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006 by Tom Albers <tomalbers at kde dot nl>
 * Copyright (C) 2006 by Michael Hoechstetter <michael dot hoechstetter at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.

#include <qtimer.h>

// KDE includes

#include <kstandarddirs.h>
#include <kdebug.h>
#include <ktempdir.h>
#include <klocale.h>
#include <kapplication.h>

// LibKipi includes.

#include <libkipi/batchprogressdialog.h>

// Local includes.

#include "imageresize.h"
#include "emailsettingscontainer.h"
#include "sendimages.h"
#include "sendimages.moc"

namespace KIPISendimagesPlugin
{

class SendImagesPriv
{
public:

    SendImagesPriv()
    {
        threadImgResize = 0;
        progressDlg     = 0;
    }

    KUrl::List                 imagesFailed2Resize;

    KIPI::BatchProgressDialog *progressDlg;

    EmailSettingsContainer     settings;

    ImageResize               *threadImgResize;
};

SendImages::SendImages(const EmailSettingsContainer& settings, QObject *parent)
          : QObject(parent)
{
    d = new SendImagesPriv;
    d->settings = settings;
    d->threadImgResize = new KIPISendimagesPlugin::ImageResize(this);

    connect(d->threadImgResize, SIGNAL(startingResize(const KUrl&)),
            this, SLOT(slotStartingResize(const KUrl&)));

    connect(d->threadImgResize, SIGNAL(finishedResize(const KUrl&, const QString&)),
            this, SLOT(slotFinishedResize(const KUrl&, const QString&)));

    connect(d->threadImgResize, SIGNAL(failedResize(const KUrl&, const QString&)),
            this, SLOT(slotFailedResize(const KUrl&, const QString&)));

    connect(d->threadImgResize, SIGNAL(completeResize()),
            this, SLOT(slotCompleteResize()));
}

SendImages::~SendImages()
{    
    delete d->progressDlg;
    delete d;
}

void SendImages::sendImages()
{
    if (!d->threadImgResize->isRunning())
    {
        d->threadImgResize->cancel();
        d->threadImgResize->wait();
    }

    KTempDir tmpDir(KStandardDirs::locateLocal("tmp", "kipiplugin-sendimages"), 0700);
    tmpDir.setAutoRemove(false);
    d->settings.tempPath = tmpDir.name();

    d->progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(), 
                               i18n("E-mail images"));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();
    d->settings.attachedfilePaths.clear();
    d->imagesFailed2Resize.clear();

    if (d->settings.imagesChangeProp)
    {
        // Resize all images if necessary in a separate threadImgResize.
        // Attachements list is updated by slotFinishedResize().

        d->threadImgResize->resize(d->settings);
        d->threadImgResize->start();
    }
    else
    {
        // Add all original files to the attachments list.

        for (QList<EmailItem>::const_iterator it = d->settings.itemsList.begin();
            it != d->settings.itemsList.end(); ++it) 
        {
            d->settings.attachedfilePaths.append((*it).url.path());
        }    

        // TODO: call second stage...
    }
}

void SendImages::slotCancel()
{
    KTempDir::removeDir(d->settings.tempPath);
}

void SendImages::slotStartingResize(const KUrl& url)
{
    QString text = i18n("Resizing %1", url.fileName());
    d->progressDlg->addedAction(text, KIPI::StartingMessage);
}

void SendImages::slotFinishedResize(const KUrl& url, const QString& resizedImgPath)
{
    kDebug() << resizedImgPath << endl;
    d->settings.attachedfilePaths.append(resizedImgPath);

    QString text = i18n("%1 resized succesfully", url.fileName());
    d->progressDlg->addedAction(text, KIPI::StartingMessage);
}

void SendImages::slotFailedResize(const KUrl& url, const QString& error)
{
    QString text = i18n("Failed to resize %1 : %2", url.fileName(), error);
    d->progressDlg->addedAction(text, KIPI::StartingMessage);

    d->imagesFailed2Resize.append(url);
}

void SendImages::slotCompleteResize()
{
        // TODO: call second stage...
}

}  // NameSpace KIPISendimagesPlugin
