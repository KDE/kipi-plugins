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
#include <QProcess>

// KDE includes

#include <kguiitem.h>
#include <ktoolinvocation.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <ktempdir.h>
#include <klocale.h>
#include <kapplication.h>
#include <kmessagebox.h>

// LibKipi includes.

#include <libkipi/batchprogressdialog.h>
#include <libkipi/interface.h>

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
        iface           = 0;
    }

    KUrl::List                 attachementFiles;
    KUrl::List                 failedResizedImages;

    KIPI::BatchProgressDialog *progressDlg;
    KIPI::Interface           *iface;

    EmailSettingsContainer     settings;

    ImageResize               *threadImgResize;
};

SendImages::SendImages(const EmailSettingsContainer& settings, QObject *parent, KIPI::Interface *iface)
          : QObject(parent)
{
    d = new SendImagesPriv;
    d->settings = settings;
    d->iface    = iface;
    d->threadImgResize = new KIPISendimagesPlugin::ImageResize(this);

    connect(d->threadImgResize, SIGNAL(startingResize(const KUrl&)),
            this, SLOT(slotStartingResize(const KUrl&)));

    connect(d->threadImgResize, SIGNAL(finishedResize(const KUrl&, const KUrl&, int)),
            this, SLOT(slotFinishedResize(const KUrl&, const KUrl&, int)));

    connect(d->threadImgResize, SIGNAL(failedResize(const KUrl&, const QString&, int)),
            this, SLOT(slotFailedResize(const KUrl&, const QString&, int)));

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
    d->progressDlg->setProgress(0, 100);
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
        d->progressDlg->setProgress(50, 100);
        secondStage();
    }
}

void SendImages::slotCancel()
{
    d->progressDlg->addedAction(i18n("Operation canceled by user"), KIPI::WarningMessage);
    d->progressDlg->setProgress(0, 100);
    d->progressDlg->setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());

    disconnect(d->progressDlg, SIGNAL(cancelClicked()),
               this, SLOT(slotCancel()));

    slotCleanUp();
}

void SendImages::slotStartingResize(const KUrl& orgUrl)
{
    QString text = i18n("Resizing %1", orgUrl.fileName());
    d->progressDlg->addedAction(text, KIPI::StartingMessage);
}

void SendImages::slotFinishedResize(const KUrl& orgUrl, const KUrl& emailUrl, int percent)
{
    d->progressDlg->setProgress((int)(80.0*(percent/100.0)), 100);
    kDebug() << emailUrl;
    d->attachementFiles.append(emailUrl);
    d->settings.setEmailUrl(orgUrl, emailUrl);

    QString text = i18n("%1 resized succesfully", orgUrl.fileName());
    d->progressDlg->addedAction(text, KIPI::SuccessMessage);
}

void SendImages::slotFailedResize(const KUrl& orgUrl, const QString& error, int percent)
{
    d->progressDlg->setProgress((int)(80.0*(percent/100.0)), 100);
    QString text = i18n("Failed to resize %1 : %2", orgUrl.fileName(), error);
    d->progressDlg->addedAction(text, KIPI::ErrorMessage);

    d->failedResizedImages.append(orgUrl);
}

void SendImages::slotCompleteResize()
{
    if (!showFailedResizedImages())
    {
        slotCancel();
        return;
    }
    secondStage();
}

void SendImages::secondStage()
{
    buildPropertiesFile();
    d->progressDlg->setProgress(90, 100);
    invokeMailAgent();
    d->progressDlg->setProgress(100, 100);
}

/** Creates a text file with all images Comments, Tags, and Rating. */
void SendImages::buildPropertiesFile()
{
    if (d->settings.addCommentsAndTags)
    {
        d->progressDlg->addedAction(i18n("Build images properties file"), KIPI::StartingMessage);
        
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

            propertiesText.append(i18n("file \"%1\":\nOriginal images: %2\n"));

            if (d->iface->hasFeature(KIPI::ImagesHasComments))
                propertiesText.append(i18n("Comments: %1\n", comments));
            
            if (d->iface->hasFeature(KIPI::HostSupportsTags))
                propertiesText.append(i18n("Tags: %1\n", tags));

            if (d->iface->hasFeature(KIPI::HostSupportsRating))
                propertiesText.append(i18n("Rating: %1\n", rating));
   
            propertiesText.append("\n");
        }

        QFile propertiesFile( d->settings.tempPath + i18n("properties.txt") );
        QTextStream stream( &propertiesFile );
        stream.setCodec(QTextCodec::codecForName("UTF-8"));
        stream.setAutoDetectUnicode(true);
        propertiesFile.open(QIODevice::WriteOnly);
        stream << propertiesText << "\n";
        propertiesFile.close();
        d->attachementFiles.append(propertiesFile.fileName());

        d->progressDlg->addedAction(i18n("Images properties file done"), KIPI::SuccessMessage);
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
            case KMessageBox::Yes:        
            {
                // Added source image files instead resized images...
                for (KUrl::List::const_iterator it = d->failedResizedImages.begin();
                    it != d->failedResizedImages.end(); ++it) 
                {
                    d->attachementFiles.append(*it);            
                    d->settings.setEmailUrl(*it, *it);
                }
                break;
            }
            case KMessageBox::No:         
            {
                // Do nothing...
                break;
            }
            case KMessageBox::Cancel:
            {
                // Stop process...
                return false;
                break;
            }
        }
    }

    return true;
}

/** Returns a list of image urls, whose sum file-size is smaller than the quota set in dialog.
    The returned list are images than we can send imediatly, and are removed from d->attachementFiles list. 
    Files wich still in d->attachementFiles need to be send by another pass.
*/
KUrl::List SendImages::divideEmails()
{
    qint64 myListSize=0;
    
    KUrl::List processedNow;            // List witch can be processed now.
    KUrl::List todoAttachement;         // Still todo list

    kDebug() << "Attachment limit: " << d->settings.attachementLimitInBytes();

    for (KUrl::List::const_iterator it = d->attachementFiles.begin();
        it != d->attachementFiles.end(); ++it) 
    {
        QFile file((*it).path());
        kDebug() << "File: " << file.fileName() << " Size: " << file.size();

        if ((myListSize + file.size()) <= d->settings.attachementLimitInBytes())
        {
            myListSize += file.size();
            processedNow.append(*it);
            kDebug() << "Current list size: " << myListSize;
        }
        else 
        {
            kDebug() << "File \"" << file.fileName() << "\" is out of attachement limit!";
            todoAttachement.append(*it);
        }
    }

    d->attachementFiles = todoAttachement;
  
    return processedNow;
}

/** Invokes mail agent. Depending on which mail agent to be used, we have different
    proceedings. Easy for every agent except of mozilla derivates */
bool SendImages::invokeMailAgent()
{
    bool       agentInvoked = false;
    KUrl::List fileList;

    do
    {
        fileList = divideEmails();

        if (!fileList.isEmpty())        
        {
            switch ((int)d->settings.emailProgram)
            {
                case EmailSettingsContainer::DEFAULT:
                {
                    KToolInvocation::invokeMailer(
                        QString::null,                     // Destination address.
                        QString::null,                     // Carbon Copy address.
                        QString::null,                     // Blind Carbon Copy address
                        QString::null,                     // Message Subject.
                        QString::null,                     // Message Body.
                        QString::null,                     // Message Body File.
                        fileList.toStringList());          // Images attachments (+ image properties file).
                
                    d->progressDlg->addedAction(i18n("Starting default KDE e-mail program..."), KIPI::StartingMessage);
                    
                    agentInvoked = true;
                    break;      
                }

                case EmailSettingsContainer::BALSA:
                {
                    QString prog("balsa");
                    QStringList args;            
                    args.append("-m");
                    args.append("mailto:");
                    for (KUrl::List::Iterator it = fileList.begin() ; it != fileList.end() ; ++it )
                    {
                        args.append("-a");
                        args.append(QFile::encodeName((*it).path()));
                    }

                    if (!QProcess::startDetached(prog, args))
                        invokeMailAgentError(prog, args);
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }

                    break;
                }

                case EmailSettingsContainer::CLAWSMAIL:
                case EmailSettingsContainer::SYLPHEED:
                case EmailSettingsContainer::SYLPHEEDCLAWS:
                {
                    QStringList args;            
                    args.append("--compose");
                    args.append("--attach");
                    for (KUrl::List::Iterator it = fileList.begin() ; it != fileList.end() ; ++it )
                    {
                        args.append(QFile::encodeName((*it).path()));
                    }

                    QString prog;
                    if (d->settings.emailProgram == EmailSettingsContainer::CLAWSMAIL)
                        prog = QString("claws-mail");
                    else if (d->settings.emailProgram == EmailSettingsContainer::SYLPHEED)
                        prog = QString("sylpheed");
                    else
                        prog = QString("sylpheed-claws");
                
                    if (!QProcess::startDetached(prog, args))
                        invokeMailAgentError(prog, args);
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }

                    break;
                }

                case EmailSettingsContainer::EVOLUTION:
                {
                    QString prog("evolution");
                    QStringList args;                    
                    QString tmp = "mailto:?subject=";
                    for (KUrl::List::Iterator it = fileList.begin() ; it != fileList.end() ; ++it )
                    {
                        tmp.append("&attach=");
                        tmp.append( QFile::encodeName((*it).path()) );
                    }
                    args.append(tmp);                
    
                    if (!QProcess::startDetached(prog, args))
                        invokeMailAgentError(prog, args);
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }

                    break;
                }

                case EmailSettingsContainer::KMAIL:
                {
                    QString prog("kmail");
                    QStringList args;            
                    for (KUrl::List::Iterator it = fileList.begin() ; it != fileList.end() ; ++it )
                    {
                        args.append("--attach");
                        args.append(QFile::encodeName((*it).path()));
                    }
                
                    if (!QProcess::startDetached(prog, args))
                        invokeMailAgentError(prog, args);
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }

                    break;
                }

                // More info about command lines options with Mozilla & co: 
                // http://www.mozilla.org/docs/command-line-args.html#Syntax_Rules

                case EmailSettingsContainer::MOZILLA:
                case EmailSettingsContainer::NETSCAPE:
                case EmailSettingsContainer::THUNDERBIRD:
                case EmailSettingsContainer::GMAILAGENT:
                {
                    QString prog;
                    if (d->settings.emailProgram == EmailSettingsContainer::MOZILLA)
                        prog = QString("mozila");
                    else if (d->settings.emailProgram == EmailSettingsContainer::NETSCAPE)
                        prog = QString("netscape");
                    else if (d->settings.emailProgram == EmailSettingsContainer::THUNDERBIRD)
                        prog = QString("thunderbird");
                    else
                        prog = QString("gmailagent");

                    QStringList args;            
                    args.append("-compose");
                    QString tmp = "attachment='";
                    for (KUrl::List::Iterator it = fileList.begin() ; it != fileList.end() ; ++it )
                    {
                        tmp.append( "file://" );
                        QString toencode=(*it).encodedPathAndQuery();
                        tmp.append(toencode);
                        tmp.append( "," );
                    }
                    tmp.remove(tmp.length()-1, 1);
                    tmp.append("'");
                    
                    args.append(tmp);
  
                    if (!QProcess::startDetached(prog, args))
                        invokeMailAgentError(prog, args);
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }
          
                    break;
                }
            }
        }
    }
    while(!fileList.isEmpty());
  
    return agentInvoked;
}

void SendImages::invokeMailAgentError(const QString& prog, const QStringList& args)
{
    kDebug() << "Command Line: " << prog << args;
    QString text = i18n("Failed to start \"%1\" program. Check your system.", prog);
    d->progressDlg->addedAction(text, KIPI::ErrorMessage);
    d->progressDlg->setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());

    disconnect(d->progressDlg, SIGNAL(cancelClicked()),
               this, SLOT(slotCancel()));

    slotCleanUp();
}

void SendImages::invokeMailAgentDone(const QString& prog, const QStringList& args)
{
    kDebug() << "Command Line: " << prog << args;
    QString text = i18n("Starting \"%1\" program...", prog);
    d->progressDlg->addedAction(text, KIPI::StartingMessage);
    d->progressDlg->setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());

    disconnect(d->progressDlg, SIGNAL(cancelClicked()),
               this, SLOT(slotCancel()));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCleanUp()));
    
    d->progressDlg->addedAction(i18n("After to have sent your images by e-mail..."), KIPI::WarningMessage);
    d->progressDlg->addedAction(i18n("Press 'Close' button to clean-up temporary files"), KIPI::WarningMessage);
}

void SendImages::slotCleanUp()
{
    KTempDir::removeDir(d->settings.tempPath);
}

}  // NameSpace KIPISendimagesPlugin
