/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#include "imgurwindow.moc"

// KDE includes

#include <kdebug.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdialog.h>

// Local includes

#include "kpmetadata.h"
#include "kpimageinfo.h"
#include "kpaboutdata.h"
#include "kpversion.h"

namespace KIPIImgurExportPlugin
{

class ImgurWindow::Private

{
public:

    Private()
    {
        webService = 0;
        widget     = 0;
    }

#ifdef OAUTH_ENABLED
    ImgurTalkerAuth* webService;
#else
    ImgurTalker*     webService;
#endif //OAUTH_ENABLED

    ImgurWidget*     widget;
};

ImgurWindow::ImgurWindow(QWidget* const /*parent*/)
    : KPToolDialog(0),
      d(new Private)
{
    d->widget     = new ImgurWidget(this);

#ifdef OAUTH_ENABLED
    d->webService = new ImgurTalkerAuth(iface(), this);
#else
    d->webService = new ImgurTalker(iface(), this);
#endif //OAUTH_ENABLED

    setMainWidget(d->widget);
    setWindowIcon(KIcon("kipi-imgur"));
    setWindowTitle(i18n("Export to imgur.com"));
    setModal(false);

    setButtons(Help | Close | User1);
    setButtonGuiItem(User1, KGuiItem(i18n("Upload"), "network-workgroup", i18n("Start upload to Imgur")));
    setDefaultButton(Close);

    enableButton(User1, !d->webService->imageQueue()->isEmpty());

    // ---------------------------------------------------------------
    // About data and help button.

    KPAboutData* const about = new KPAboutData(ki18n("Imgur Export"),
                                   0,
                                   KAboutData::License_GPL,
                                   ki18n("A tool to export images to Imgur web service"),
                                   ki18n("(c) 2012-2013, Marius Orcsik"));

    about->addAuthor(ki18n("Marius Orcsik"),
                     ki18n("Author"),
                     "marius at habarnam dot ro");

    about->addAuthor(ki18n("Gilles Caulier"),
                     ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->setHandbookEntry("imgurexport");
    setAboutData(about);

    // ------------------------------------------------------------

    connect(this, SIGNAL(buttonClicked(KDialog::ButtonCode)),
            this, SLOT(slotButtonClicked(KDialog::ButtonCode)));

    connect(d->webService, SIGNAL(signalQueueChanged()),
            this, SLOT(slotImageQueueChanged()));

    connect(d->webService, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(d->webService, SIGNAL(signalUploadStart(KUrl)),
            d->widget, SLOT(slotImageUploadStart(KUrl)));

    connect(d->webService, SIGNAL(signalError(KUrl,ImgurError)),
            d->widget, SLOT(slotImageUploadError(KUrl,ImgurError)));

    connect(d->webService, SIGNAL(signalSuccess(KUrl,ImgurSuccess)),
            d->widget, SLOT(slotImageUploadSuccess(KUrl,ImgurSuccess)));

    // this signal/slot controls if the webservice should continue upload or not
    connect(d->webService, SIGNAL(signalError(KUrl,ImgurError)),
            this, SLOT(slotAddPhotoError(KUrl,ImgurError)));

    connect(d->webService, SIGNAL(signalSuccess(KUrl,ImgurSuccess)),
            this, SLOT(slotAddPhotoSuccess(KUrl,ImgurSuccess)));

    connect(this, SIGNAL(signalContinueUpload(bool)),
            d->webService, SLOT(slotContinueUpload(bool)));

    // adding/removing items from the image list
    connect(d->widget, SIGNAL(signalAddItems(KUrl::List)),
            d->webService, SLOT(slotAddItems(KUrl::List)));

    connect(d->widget, SIGNAL(signalRemoveItems(KUrl::List)),
            d->webService, SLOT(slotRemoveItems(KUrl::List)));

   // ---------------------------------------------------------------
#ifdef OAUTH_ENABLED
    connect(d->widget, SIGNAL(signalClickedChangeUser()),
            d->webService, SLOT(slotOAuthLogin()));
/*
    connect(d->webService, SIGNAL(signalAuthenticated(bool)),
            d->widget, SLOT(slotAuthenticated(bool)));
*/
    connect(d->webService, SIGNAL(signalAuthenticated(bool,QString)),
            d->widget, SLOT(slotAuthenticated(bool,QString)));

    connect(d->webService, SIGNAL(signalAuthenticated(bool,QString)),
            this, SLOT(slotAuthenticated(bool,QString)));
#endif //OAUTH_ENABLED
    readSettings();
}

ImgurWindow::~ImgurWindow()
{
    saveSettings();
    delete d;
}

void ImgurWindow::slotButtonClicked(KDialog::ButtonCode button)
{
    switch (button)
    {
        case KDialog::User1:
            emit signalContinueUpload(true);
            break;
        case KDialog::Close:
            emit signalContinueUpload(false);
            // Must cancel the transfer
            d->webService->cancel();
            d->webService->imageQueue()->clear();

            d->widget->imagesList()->cancelProcess();
            d->widget->progressBar()->setVisible(false);
            d->widget->progressBar()->progressCompleted();

            // close the dialog
            d->widget->imagesList()->listView()->clear();

            done(Close);
            break;
        default:
            break;
    }
}

void ImgurWindow::reactivate()
{
    d->widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void ImgurWindow::slotImageQueueChanged()
{
    enableButton(User1, !d->webService->imageQueue()->isEmpty());
}

void ImgurWindow::slotAddPhotoError(const KUrl& /*currentImage*/, const ImgurError& error)
{
    if (!d->webService->imageQueue()->isEmpty())
    {
        if (KMessageBox::warningContinueCancel(this,
                                               i18n("Failed to upload photo to Imgur: %1\n"
                                                    "Do you want to continue?", error.message))
            == KMessageBox::Continue)
        {
            emit signalContinueUpload(true);
        }
        else
        {
            emit signalContinueUpload(false);
        }

    }
    else
    {
        KMessageBox::sorry(this,i18n("Failed to upload photo to Imgur: %1\n", error.message));
    }

    return;
}

void ImgurWindow::slotAddPhotoSuccess(const KUrl& /*currentImage*/, const ImgurSuccess& /*success*/)
{
   emit signalContinueUpload(true);
}

void ImgurWindow::slotAuthenticated(bool yes, const QString& message)
{
    QString err;

    if (!message.isEmpty())
    {
        err = i18nc("%1 is the error string",
                    "Failed to authenticate to Imgur.\n%1\nDo you want to continue?",
                    message);
    }
    else
    {
        err = i18n("Failed to authenticate to Imgur.\nDo you want to continue?");
    }

    if (yes)
    {
        enableButton(User1, yes);
    }
    else if (KMessageBox::warningContinueCancel(this, err)
               == KMessageBox::Continue)
    {
        enableButton(User1, true);
    }
}

void ImgurWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        enableButton(User1, false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        enableButton(User1, !d->webService->imageQueue()->isEmpty());
    }
}

void ImgurWindow::closeEvent(QCloseEvent*)
{
    saveSettings();
}

void ImgurWindow::readSettings()
{
    KConfig config("kipirc");
    //KConfigGroup group = config.group(QString("Imgur Settings"));

    KConfigGroup group2 = config.group(QString("Imgur Dialog"));
    restoreDialogSize(group2);
}

void ImgurWindow::saveSettings()
{
    KConfig config("kipirc");
    //KConfigGroup group = config.group(QString("Imgur Settings"));

    KConfigGroup group2 = config.group(QString("Imgur Dialog"));
    saveDialogSize(group2);
    config.sync();
}

} // namespace KIPIImgurExportPlugin
