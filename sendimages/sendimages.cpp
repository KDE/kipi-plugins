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

#include <ktoolinvocation.h>
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

        secondStage();
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
    if (!showFailedResizedImages()) return;
    secondStage();
}

void SendImages::secondStage()
{
    buildPropertiesFile();
    invokeMailAgent();
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

/** Returns a list of image urls, whose sum file-size is smaller than the quota set in dialog.
    The returned list are images than we can send imediatly, and are removed from d->attachementFiles list. 
    Files wich still in d->attachementFiles need to be send by another pass.
*/
KUrl::List SendImages::divideEmails()
{
    qint64 myListSize=0;
    
    KUrl::List processedNow;            // files witch can be processed now.
    KUrl::List todoAttachement;          // Still todo list

    kDebug() << "Attachment limit: " << d->settings.attachementLimitInBytes() << endl;

    for (KUrl::List::const_iterator it = d->attachementFiles.begin();
        it != d->attachementFiles.end(); ++it) 
    {
        QFile file((*it).path());
        kDebug() << "File: " << file.fileName() << " Size: " << file.size() << endl;

        if ((myListSize + file.size()) <= d->settings.attachementLimitInBytes())
        {
            myListSize += file.size();
            processedNow.append(*it);
            kDebug() << "Current list size: " << myListSize << endl;
        }
        else 
        {
            kDebug() << "File \"" << file.fileName() << "\" is out of attachement limit!" << endl;
            todoAttachement.append(*it);
        }
    }

    d->attachementFiles = todoAttachement;
  
    return processedNow;
}

/** Shows up an error dialog about the images wich are over the attachment limit. */
bool SendImages::showImagesOverAttachementLimit(const KUrl::List& removedFiles)
{
    if (removedFiles.isEmpty())
    {
        QStringList list;
        for (KUrl::List::const_iterator it = removedFiles.begin();
            it != removedFiles.end(); ++it) 
        {
            list.append((*it).fileName());
        }
        
        int valRet = KMessageBox::warningYesNoList(kapp->activeWindow(), 
                                  i18n("The images are bigger than attachement limit and will not sent\n"
                                       "Do you want to continue ?"), 
                                  list, 
                                  i18n("Failed to attach images"));

        switch (valRet)
        {
            case KMessageBox::Yes :       
                break;
        
            case KMessageBox::No :      
                slotCancel();
                return false;
                break;
        }
    }

    return true;
}

/** Invokes mail agent. Depending on which mail agent to be used, we have different
    proceedings. Easy for every agent except of mozilla derivates */
bool SendImages::invokeMailAgent()
{
    bool agentInvoked = false;
    d->progressDlg->addedAction(i18n("Invoke e-mail program"), KIPI::StartingMessage);

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
                    
                    agentInvoked = true;
                    break;      
                }
            }
        }
    }
    while(!fileList.isEmpty());

/*        
        // KMail mail agent call.
    
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "KMail" )
        {
            m_mailAgentProc = new KProcess;
            *m_mailAgentProc << "kmail";
    
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
            {
                *m_mailAgentProc << "--attach";
                *m_mailAgentProc << QFile::encodeName((*it).path());
            }
        
            if ( m_mailAgentProc->start() == false )
                KMessageBox::error(kapp->activeWindow(), 
                                i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
                agentInvoked = true;
        }

    
        // Claws Mail and Sylpheed mail agent call.
    
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Claws Mail" ||
             m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed" ||
             m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed-Claws" )
        {
            m_mailAgentProc = new KProcess;

            if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Claws Mail")
                *m_mailAgentProc << "claws-mail";
            else if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed")
                *m_mailAgentProc << "sylpheed";
            else
                *m_mailAgentProc << "sylpheed-claws";
        
            *m_mailAgentProc << "--compose" << "--attach";
        
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
                *m_mailAgentProc << QFile::encodeName((*it).path());
        
            if ( m_mailAgentProc->start() == false )
                KMessageBox::error(kapp->activeWindow(), 
                                   i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
                agentInvoked = true;
        }
    
        // Balsa mail agent call.
    
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Balsa" )
        {
            m_mailAgentProc = new KProcess;
            *m_mailAgentProc << "balsa" << "-m" << "mailto:";
        
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
            {
                *m_mailAgentProc << "-a";
                *m_mailAgentProc << QFile::encodeName((*it).path());
            }
        
            if ( m_mailAgentProc->start() == false )
                KMessageBox::error(kapp->activeWindow(), 
                                   i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
                agentInvoked = true;
        }
    
        // Evolution mail agent call.

        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Evolution" )    
        {
            m_mailAgentProc = new KProcess;
            *m_mailAgentProc << "evolution";
        
            QString Temp = "mailto:?subject=";
        
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
            {
                Temp.append("&attach=");
                Temp.append( QFile::encodeName((*it).path()) );
            }
        
            *m_mailAgentProc << Temp;
        
            if ( m_mailAgentProc->start() == false )
                KMessageBox::error(kapp->activeWindow(), 
                                   i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
                agentInvoked = true;
        }
    
        // Mozilla | Netscape | Thunderbird mail agent call.
    
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Mozilla" ||
             m_sendImagesDialog->m_mailAgentName->currentText() == "Netscape" ||
             m_sendImagesDialog->m_mailAgentName->currentText() == "Thunderbird" ||
             m_sendImagesDialog->m_mailAgentName->currentText() == "GmailAgent")
        {
            m_mailAgentProc = new KProcess;
        
            m_thunderbirdUrl = m_sendImagesDialog->m_ThunderbirdBinPath->url();
        
            if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Mozilla" )
            {
                *m_mailAgentProc << "mozilla" << "-remote";
            }
            else if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Thunderbird" )
            {
                *m_mailAgentProc << m_thunderbirdUrl << "-remote";
                qDebug("URL: %s", m_thunderbirdUrl.ascii());
            }
            else if ( m_sendImagesDialog->m_mailAgentName->currentText() == "GmailAgent" )
            {
                *m_mailAgentProc << "gmailagent" << "-remote";
            }
            else
            {
                *m_mailAgentProc << "netscape" << "-remote";
            }
        
            QString Temp = " xfeDoCommand(composeMessage,attachment='";
        
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
            {
                Temp.append( "file://" );
                QString toencode=(*it).encodedPathAndQuery();
                Temp.append(toencode);
                Temp.append( "," );
            }
    
            Temp.remove(Temp.length()-1,1);
            Temp.append("')");
            
            *m_mailAgentProc << Temp;
       
            if (!m_invokedBefore)
            {
                connect(m_mailAgentProc, SIGNAL(processExited(KProcess *)),
                        this, SLOT(slotMozillaExited(KProcess*)));
        
                connect(m_mailAgentProc, SIGNAL(receivedStderr(KProcess *, char*, int)),
                        this, SLOT(slotMozillaReadStderr(KProcess*, char*, int)));
            }
            qDebug ("%s", Temp.ascii());
        
            if ( m_mailAgentProc->start(KProcess::NotifyOnExit , KProcess::All) == false )
                KMessageBox::error(kapp->activeWindow(), 
                                   i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
            {   agentInvoked = true;
                m_invokedBefore=true;
            }
        }
*/
  
    return agentInvoked;
}

}  // NameSpace KIPISendimagesPlugin
