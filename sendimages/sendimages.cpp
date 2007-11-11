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

#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

// KDE includes

#include <kstandarddirs.h>
#include <kdebug.h>
#include <ktempdir.h>
#include <klocale.h>
#include <kapplication.h>
#include <kmessagebox.h>

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

    KUrl::List                 attachementFiles;
    KUrl::List                 failedResizedImages;

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

    connect(d->threadImgResize, SIGNAL(finishedResize(const KUrl&, const KUrl&)),
            this, SLOT(slotFinishedResize(const KUrl&, const KUrl&)));

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
    d->attachementFiles.clear();
    d->failedResizedImages.clear();

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
            d->attachementFiles.append((*it).orgUrl);            
            d->settings.setEmailUrl((*it).orgUrl, (*it).orgUrl);
        }    

        // TODO: call second stage...
    }
}

void SendImages::slotCancel()
{
    KTempDir::removeDir(d->settings.tempPath);
}

void SendImages::slotStartingResize(const KUrl& orgUrl)
{
    QString text = i18n("Resizing %1", orgUrl.fileName());
    d->progressDlg->addedAction(text, KIPI::StartingMessage);
}

void SendImages::slotFinishedResize(const KUrl& orgUrl, const KUrl& emailUrl)
{
    kDebug() << emailUrl << endl;
    d->attachementFiles.append(emailUrl);
    d->settings.setEmailUrl(orgUrl, emailUrl);

    QString text = i18n("%1 resized succesfully", orgUrl.fileName());
    d->progressDlg->addedAction(text, KIPI::SuccessMessage);
}

void SendImages::slotFailedResize(const KUrl& orgUrl, const QString& error)
{
    QString text = i18n("Failed to resize %1 : %2", orgUrl.fileName(), error);
    d->progressDlg->addedAction(text, KIPI::ErrorMessage);

    d->failedResizedImages.append(orgUrl);
}

void SendImages::slotCompleteResize()
{
    // TODO: call second stage...
}

/** Creates a text file with all images Comments, Tags, and Rating. */
void SendImages::buildPropertiesFile()
{
    if (d->settings.addCommentsAndTags)
    {
        QString propertiesText;
        
        for (QList<EmailItem>::const_iterator it = d->settings.itemsList.begin();
            it != d->settings.itemsList.end(); ++it) 
        {
            EmailItem item    = *it;
            QString comments  = item.comments;
            QString tags      = item.tags.join(", ");
            QString rating    = QString::number(item.rating);
            QString orgFile   = item.orgUrl.fileName();
            QString emailFile = item.emailUrl.fileName();

            if (comments.isEmpty())
                comments = i18n("no caption");

            if (tags.isEmpty())
                tags = i18n("no keywords");

            propertiesText += i18n("file \"%1\":\nOriginal images: %2\nComments: %3\nTags: %4\nRating: %5\n\n",
                                   emailFile, orgFile, comments, tags, rating);
        }

        QFile propertiesFile( d->settings.tempPath + i18n("properties.txt") );
        QTextStream stream( &propertiesFile );
        stream.setCodec(QTextCodec::codecForName("UTF-8"));
        stream.setAutoDetectUnicode(true);
        propertiesFile.open(QIODevice::WriteOnly);
        stream << propertiesText << "\n";
        propertiesFile.close();
        d->attachementFiles.append(propertiesFile.fileName());
    }
}

/** Shows up an error dialog about the problematic resized images. */
bool SendImages::showFailedResizedImages()
{
    if (!d->failedResizedImages.isEmpty())
    {
        QStringList list;
        for (KUrl::List::const_iterator it = d->failedResizedImages.begin();
            it != d->failedResizedImages.end(); ++it) 
        {
            list.append((*it).fileName());
        }
        
        int valRet = KMessageBox::warningYesNoCancelList(kapp->activeWindow(), 
                                  i18n("The images listed below cannot be resized.\n"
                                       "Do you want them to be added as attachments "
                                       "(without resizing)?"), 
                                  list, 
                                  i18n("Failed to resize images"));

        switch (valRet)
        {
            case KMessageBox::Yes :        // Added source image files instead resized images...
        
                for (KUrl::List::const_iterator it = d->failedResizedImages.begin();
                    it != d->failedResizedImages.end(); ++it) 
                {
                    d->attachementFiles.append(*it);            
                    d->settings.setEmailUrl(*it, *it);
                }
                break;
        
            case KMessageBox::No :         // Do nothing...
                break;
        
            case KMessageBox::Cancel :     // Stop process...
                slotCancel();
                return false;
                break;
        }
    }

    return true;
}

}  // NameSpace KIPISendimagesPlugin
