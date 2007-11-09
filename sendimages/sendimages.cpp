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

#include "actionthread.h"
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
        thread      = 0;
        progressDlg = 0;
    }

    KIPI::BatchProgressDialog *progressDlg;

    EmailSettingsContainer     settings;

    ActionThread              *thread;
};

SendImages::SendImages(const EmailSettingsContainer& settings, QObject *parent)
          : QObject(parent)
{
    d = new SendImagesPriv;
    d->settings = settings;
    d->thread = new KIPISendimagesPlugin::ActionThread(this);

    connect(d->thread, SIGNAL(startingResize(const KUrl&)),
            this, SLOT(slotStartingResize(const KUrl&)));

    connect(d->thread, SIGNAL(finishedResize(const KUrl&, const QString&)),
            this, SLOT(slotFinishedResize(const KUrl&, const QString&)));

    connect(d->thread, SIGNAL(failedResize(const KUrl&, const QString&)),
            this, SLOT(slotFailedResize(const KUrl&, const QString&)));

    connect(d->thread, SIGNAL(completeResize()),
            this, SLOT(slotCompleteResize()));
}

SendImages::~SendImages()
{    
    delete d->progressDlg;
    delete d;
}

void SendImages::sendImages()
{
    if (!d->thread->isRunning())
    {
        d->thread->cancel();
        d->thread->wait();
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

    // Resize all images if necessary

    if (d->settings.imagesChangeProp)
    {
        d->thread->resize(d->settings);
        d->thread->start();
    }
    else
    {
        // TODO
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
}

void SendImages::slotCompleteResize()
{
}

}  // NameSpace KIPISendimagesPlugin
