/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-02-25
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2004-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2006      by Tom Albers <tomalbers at kde dot nl>
 * Copyright (C) 2006      by Michael Hoechstetter <michael dot hoechstetter at gmx dot de>
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

#include "sendimages.h"

// Qt includes

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>
#include <QApplication>
#include <QTemporaryDir>
#include <QMessageBox>

// KDE includes

#include <kstandardguiitem.h>
#include <klocalizedstring.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kmessagebox.h>

// Libkipi includes

#include <interface.h>
#include <pluginloader.h>

// Local includes

#include "kpbatchprogressdialog.h"
#include "kipiplugins_debug.h"
#include "imageresize.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPISendimagesPlugin
{

class SendImages::Private
{
public:

    Private()
    {
        cancel                 = false;
        threadImgResize        = 0;
        progressDlg            = 0;
        iface                  = 0;
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

    bool                   cancel;

    QList<QUrl>             attachementFiles;
    QList<QUrl>             failedResizedImages;

    Interface*             iface;

    KPBatchProgressDialog* progressDlg;

    EmailSettings          settings;

    ImageResize*           threadImgResize;
};

SendImages::SendImages(const EmailSettings& settings, QObject* const parent)
    : QObject(parent), d(new Private)
{
    d->settings        = settings;
    d->threadImgResize = new ImageResize(this);

    connect(d->threadImgResize, SIGNAL(startingResize(QUrl)),
            this, SLOT(slotStartingResize(QUrl)));

    connect(d->threadImgResize, SIGNAL(finishedResize(QUrl,QUrl,int)),
            this, SLOT(slotFinishedResize(QUrl,QUrl,int)));

    connect(d->threadImgResize, SIGNAL(failedResize(QUrl,QString,int)),
            this, SLOT(slotFailedResize(QUrl,QString,int)));

    connect(d->threadImgResize, SIGNAL(finished()),
            this, SLOT(slotCompleteResize()));
}

SendImages::~SendImages()
{
    delete d->progressDlg;
    delete d;
}

void SendImages::firstStage()
{
    d->cancel = false;

    if (!d->threadImgResize->isRunning())
    {
        d->threadImgResize->cancel();
        d->threadImgResize->wait();
    }

    QTemporaryDir tmpDir(KStandardDirs::locateLocal("tmp", "kipiplugin-sendimages"));
    tmpDir.setAutoRemove(false);
    d->settings.tempPath = tmpDir.path();

    QDir tmp(d->settings.tempPath);
    QStringList folders = tmp.absolutePath().split('/', QString::SkipEmptyParts);

    if (!folders.isEmpty())
    {
        d->settings.tempFolderName = folders.last();
    }

    d->progressDlg = new KPBatchProgressDialog(QApplication::activeWindow(), i18n("Email images"));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();
    d->progressDlg->progressWidget()->setProgress(0, 100);
    d->attachementFiles.clear();
    d->failedResizedImages.clear();

    if (d->settings.imagesChangeProp)
    {
        // Resize all images if necessary in a separate threadImgResize.
        // Attachments list is updated by slotFinishedResize().

        d->threadImgResize->resize(d->settings);
        d->threadImgResize->start();
    }
    else
    {
        // Add all original files to the attachments list.

        foreach(const EmailItem& item, d->settings.itemsList)
        {
            d->attachementFiles.append(item.orgUrl);
            d->settings.setEmailUrl(item.orgUrl, item.orgUrl);
        }

        d->progressDlg->progressWidget()->setProgress(50, 100);
        secondStage();
    }
}

void SendImages::slotCancel()
{
    d->cancel = true;

    if (!d->threadImgResize->isRunning())
    {
        d->threadImgResize->cancel();
        d->threadImgResize->wait();
    }

    d->progressDlg->progressWidget()->addedAction(i18n("Operation canceled by user"), WarningMessage);
    d->progressDlg->progressWidget()->setProgress(0, 100);
    d->progressDlg->setButtonClose();

    disconnect(d->progressDlg, SIGNAL(cancelClicked()),
               this, SLOT(slotCancel()));

    slotCleanUp();
}

void SendImages::slotStartingResize(const QUrl& orgUrl)
{
    if (d->cancel) return;

    QString text = i18n("Resizing %1", orgUrl.fileName());
    d->progressDlg->progressWidget()->addedAction(text, StartingMessage);
}

void SendImages::slotFinishedResize(const QUrl& orgUrl, const QUrl& emailUrl, int percent)
{
    if (d->cancel) return;

    d->progressDlg->progressWidget()->setProgress((int)(80.0*(percent/100.0)), 100);
    qCDebug(KIPIPLUGINS_LOG) << emailUrl;
    d->attachementFiles.append(emailUrl);
    d->settings.setEmailUrl(orgUrl, emailUrl);

    QString text = i18n("%1 resized successfully", orgUrl.fileName());
    d->progressDlg->progressWidget()->addedAction(text, SuccessMessage);
}

void SendImages::slotFailedResize(const QUrl& orgUrl, const QString& error, int percent)
{
    if (d->cancel) return;

    d->progressDlg->progressWidget()->setProgress((int)(80.0*(percent/100.0)), 100);
    QString text = i18n("Failed to resize %1: %2", orgUrl.fileName(), error);
    d->progressDlg->progressWidget()->addedAction(text, ErrorMessage);

    d->failedResizedImages.append(orgUrl);
}

void SendImages::slotCompleteResize()
{
    if (d->cancel) return;

    if (!showFailedResizedImages())
    {
        slotCancel();
        return;
    }

    secondStage();
}

void SendImages::secondStage()
{
    if (d->cancel) return;

    // If the initial list of files contained only unsupported file formats,
    // and the user chose not to attach them without resizing, then there are
    // no files approved for sending.
    if (d->attachementFiles.isEmpty())
    {
        d->progressDlg->progressWidget()->addedAction(i18n("There are no files to send"), WarningMessage);
        d->progressDlg->progressWidget()->setProgress(0, 100);
        d->progressDlg->setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());
        return;
    }

    buildPropertiesFile();
    d->progressDlg->progressWidget()->setProgress(90, 100);
    invokeMailAgent();
    d->progressDlg->progressWidget()->setProgress(100, 100);
}

void SendImages::buildPropertiesFile()
{
    if (d->cancel) return;

    if (d->settings.addCommentsAndTags)
    {
        d->progressDlg->progressWidget()->addedAction(i18n("Build images properties file"), StartingMessage);

        QString propertiesText;

        foreach(const EmailItem& item, d->settings.itemsList)
        {
            QString comments  = item.comments;
            QString tags      = item.tags.join(", ");
            QString rating    = QString::number(item.rating);
            QString orgFile   = item.orgUrl.fileName();
            QString emailFile = item.emailUrl.fileName();

            if (comments.isEmpty())
                comments = i18n("no caption");

            if (tags.isEmpty())
                tags = i18n("no keywords");

            propertiesText.append(i18n("file \"%1\":\nOriginal images: %2\n", emailFile, orgFile));

            if (d->iface->hasFeature(ImagesHasComments))
                propertiesText.append(i18n("Comments: %1\n", comments));

            if (d->iface->hasFeature(HostSupportsTags))
                propertiesText.append(i18n("Tags: %1\n", tags));

            if (d->iface->hasFeature(HostSupportsRating))
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
        d->attachementFiles << QUrl(propertiesFile.fileName());

        d->progressDlg->progressWidget()->addedAction(i18n("Image properties file done"), SuccessMessage);
    }
}

bool SendImages::showFailedResizedImages() const
{
    if (!d->failedResizedImages.isEmpty())
    {
        QStringList list;

        for (QList<QUrl>::const_iterator it = d->failedResizedImages.constBegin();
            it != d->failedResizedImages.constEnd(); ++it)
        {
            list.append((*it).fileName());
        }

        int valRet = KMessageBox::warningYesNoCancelList(QApplication::activeWindow(),
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
                for (QList<QUrl>::const_iterator it = d->failedResizedImages.constBegin();
                    it != d->failedResizedImages.constEnd(); ++it)
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

QList<QUrl> SendImages::divideEmails() const
{
    qint64 myListSize = 0;

    QList<QUrl> processedNow;            // List witch can be processed now.
    QList<QUrl> todoAttachement;         // Still todo list

    qCDebug(KIPIPLUGINS_LOG) << "Attachment limit: " << d->settings.attachementLimitInBytes();

    for (QList<QUrl>::const_iterator it = d->attachementFiles.constBegin();
        it != d->attachementFiles.constEnd(); ++it)
    {
        QFile file((*it).path());
        qCDebug(KIPIPLUGINS_LOG) << "File: " << file.fileName() << " Size: " << file.size();

        if ((myListSize + file.size()) <= d->settings.attachementLimitInBytes())
        {
            myListSize += file.size();
            processedNow.append(*it);
            qCDebug(KIPIPLUGINS_LOG) << "Current list size: " << myListSize;
        }
        else
        {
            if ((file.size()) >= d->settings.attachementLimitInBytes())
            {
                qCDebug(KIPIPLUGINS_LOG) << "File \"" << file.fileName() << "\" is out of attachment limit!";
                QString text = i18n("The file \"%1\" is too big to be sent, please reduce its size or change your settings" , file.fileName());
                d->progressDlg->progressWidget()->addedAction(text, WarningMessage);
            }
            else
            {
                todoAttachement.append(*it);
            }
        }
    }

    d->attachementFiles = todoAttachement;

    return processedNow;
}

bool SendImages::invokeMailAgent()
{
    if (d->cancel) return false;

    bool       agentInvoked = false;
    QList<QUrl> fileList;

    do
    {
        fileList = divideEmails();

        if (!fileList.isEmpty())
        {
            QStringList stringFileList;

            foreach(const QUrl& file, fileList)
            {
                stringFileList << file.path();
            }

            switch ((int)d->settings.emailProgram)
            {
                case EmailSettings::DEFAULT:
                {
                    KToolInvocation::invokeMailer(
                        QString(),                     // Destination address.
                        QString(),                     // Carbon Copy address.
                        QString(),                     // Blind Carbon Copy address
                        QString(),                     // Message Subject.
                        QString(),                     // Message Body.
                        QString(),                     // Message Body File.
                        stringFileList);               // Images attachments (+ image properties file).

                    d->progressDlg->progressWidget()->addedAction(i18n("Starting default KDE email program..."), StartingMessage);

                    agentInvoked = true;
                    break;
                }

                case EmailSettings::BALSA:
                {
                    QString prog("balsa");
                    QStringList args;

#ifdef _WIN32
                    args.append("/c");
                    args.append("start");
                    args.append(prog);
                    prog = QString("cmd");
#endif

                    args.append("-m");
                    args.append("mailto:");

                    for (QList<QUrl>::ConstIterator it = fileList.constBegin() ; it != fileList.constEnd() ; ++it )
                    {
                        args.append("-a");
                        args.append((*it).path());
                    }

                    if (!QProcess::startDetached(prog, args))
                    {
                        invokeMailAgentError(prog, args);
                    }
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }

                    break;
                }

                case EmailSettings::CLAWSMAIL:
                case EmailSettings::SYLPHEED:
                case EmailSettings::SYLPHEEDCLAWS:
                {
                    QStringList args;
                    QString     prog;

#ifdef _WIN32
                    args.append("/c");
                    args.append("start");
                    args.append(prog);
                    prog = QString("cmd");
#endif

                    args.append("--compose");
                    args.append("--attach");

                    for (QList<QUrl>::ConstIterator it = fileList.constBegin() ; it != fileList.constEnd() ; ++it )
                    {
                        args.append((*it).path());
                    }

                    if (d->settings.emailProgram == EmailSettings::CLAWSMAIL)
                    {
                        prog = QString("claws-mail");
                    }
                    else if (d->settings.emailProgram == EmailSettings::SYLPHEED)
                    {
                        prog = QString("sylpheed");
                    }
                    else
                    {
                        prog = QString("sylpheed-claws");
                    }

                    if (!QProcess::startDetached(prog, args))
                    {
                        invokeMailAgentError(prog, args);
                    }
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }

                    break;
                }

                case EmailSettings::EVOLUTION:
                {
                    QString prog("evolution");
                    QStringList args;

#ifdef _WIN32
                    args.append("/c");
                    args.append("start");
                    args.append(prog);
                    prog = QString("cmd");
#endif

                    QString tmp = "mailto:?subject=";

                    for (QList<QUrl>::ConstIterator it = fileList.constBegin() ; it != fileList.constEnd() ; ++it )
                    {
                        tmp.append("&attach=");
                        tmp.append( (*it).path() );
                    }

                    args.append(tmp);

                    if (!QProcess::startDetached(prog, args))
                    {
                        invokeMailAgentError(prog, args);
                    }
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }

                    break;
                }

                case EmailSettings::KMAIL:
                {
                    QString prog("kmail");
                    QStringList args;

#ifdef _WIN32
                    args.append("/c");
                    args.append("start");
                    args.append(prog);
                    prog = QString("cmd");
#endif

                    for (QList<QUrl>::ConstIterator it = fileList.constBegin() ; it != fileList.constEnd() ; ++it )
                    {
                        args.append("--attach");
                        args.append((*it).path());
                    }

                    if (!QProcess::startDetached(prog, args))
                    {
                        invokeMailAgentError(prog, args);
                    }
                    else
                    {
                        invokeMailAgentDone(prog, args);
                        agentInvoked = true;
                    }

                    break;
                }

                // More info about command lines options with Mozilla & co:
                // http://www.mozilla.org/docs/command-line-args.html#Syntax_Rules

                case EmailSettings::NETSCAPE:
                case EmailSettings::THUNDERBIRD:
                case EmailSettings::GMAILAGENT:
                {
                    QString prog;

                    if (d->settings.emailProgram == EmailSettings::NETSCAPE)
                    {
                        prog = QString("netscape");
                    }
                    else if (d->settings.emailProgram == EmailSettings::THUNDERBIRD)
                    {
                        prog = QString("thunderbird");
                    }
                    else
                    {
                        prog = QString("gmailagent");
                    }

                    QStringList args;

#ifdef _WIN32
                    args.append("/c");
                    args.append("start");
                    args.append(prog);
                    prog = QString("cmd");
#endif
                    args.append("-compose");
                    QString tmp = "attachment='";

                    for (QList<QUrl>::ConstIterator it = fileList.constBegin() ; it != fileList.constEnd() ; ++it )
                    {
                        tmp.append( "file://" );
                        tmp.append((*it).path());
                        tmp.append( "," );
                    }

                    tmp.remove(tmp.length()-1, 1);
                    tmp.append("'");

                    args.append(tmp);

                    if (!QProcess::startDetached(prog, args))
                    {
                        invokeMailAgentError(prog, args);
                    }
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
    qCDebug(KIPIPLUGINS_LOG) << "Command Line: " << prog << args;
    QString text = i18n("Failed to start \"%1\" program. Check your system.", prog);
    d->progressDlg->progressWidget()->addedAction(text, ErrorMessage);
    d->progressDlg->setButtonClose();

    disconnect(d->progressDlg, SIGNAL(cancelClicked()),
               this, SLOT(slotCancel()));

    slotCleanUp();
}

void SendImages::invokeMailAgentDone(const QString& prog, const QStringList& args)
{
    qCDebug(KIPIPLUGINS_LOG) << "Command Line: " << prog << args;
    QString text = i18n("Starting \"%1\" program...", prog);
    d->progressDlg->progressWidget()->addedAction(text, StartingMessage);
    d->progressDlg->setButtonClose();

    disconnect(d->progressDlg, SIGNAL(cancelClicked()),
               this, SLOT(slotCancel()));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCleanUp()));

    d->progressDlg->progressWidget()->addedAction(i18n("After having sent your images by email..."), WarningMessage);
    d->progressDlg->progressWidget()->addedAction(i18n("Press 'Close' button to clean up temporary files"), WarningMessage);
}

void SendImages::slotCleanUp()
{
    QDir((d->settings.tempPath)).removeRecursively();
}

}  // namespace KIPISendimagesPlugin
