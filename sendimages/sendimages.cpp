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
#include <ktempdir.h>
#include <klocale.h>
#include <kapplication.h>

// LibKipi includes.

#include <libkipi/batchprogressdialog.h>

// Local includes.

#include "actions.h"
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

    connect(d->thread, SIGNAL(starting(const KUrl&, int)),
            this, SLOT(slotStarting(const KUrl&, int)));

    connect(d->thread, SIGNAL(finished(const KUrl&, int)),
            this, SLOT(slotFinished(const KUrl&, int)));

    connect(d->thread, SIGNAL(failed(const KUrl&, int, const QString&)),
            this, SLOT(slotFailed(const KUrl&, int, const QString&)));

    connect(d->thread, SIGNAL(complete(int)),
            this, SLOT(slotComplete(int)));
}

SendImages::~SendImages()
{    
    delete d->progressDlg;
    delete d;
}

void SendImages::sendImages()
{
    KTempDir tmpDir(KStandardDirs::locateLocal("tmp", "kipiplugin-sendimages"));
    tmpDir.setAutoRemove(false);
    d->settings.tempPath = tmpDir.name();

    d->progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(), 
                               i18n("E-mail images"));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();

    // Resize all images if necessary

    if (d->settings.imagesChangeProp)
    {
        d->thread->resize(d->settings);
        if (!d->thread->isRunning())
            d->thread->start();
    }
    else
    {
        // TODO
    }
}

void SendImages::slotCancel()
{
        // TODO
}

void SendImages::slotStarting(const KUrl& url, int)
{
    QString text = i18n("Resizing %1", url.fileName());
    d->progressDlg->addedAction(text, KIPI::StartingMessage);
}

void SendImages::slotFinished(const KUrl& url, int)
{
    QString text = i18n("%1 resized succesfully", url.fileName());
    d->progressDlg->addedAction(text, KIPI::StartingMessage);
}

void SendImages::slotFailed(const KUrl& url, int, const QString& error)
{
    QString text = i18n("Failed to resize %1 : %2", url.fileName(), error);
    d->progressDlg->addedAction(text, KIPI::StartingMessage);
}

void SendImages::slotComplete(int)
{
}

}  // NameSpace KIPISendimagesPlugin
