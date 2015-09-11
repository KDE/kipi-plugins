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

#include "imgurwindow.h"

// Qt includes

#include <QCloseEvent>

// KDE includes

#include <kmessagebox.h>
#include <klocalizedstring.h>
#include <kconfig.h>
#include <KWindowConfig>

// Local includes

#include "kipiplugins_debug.h"
#include "kpmetadata.h"
#include "kpimageinfo.h"
#include "kpaboutdata.h"
#include "kpversion.h"

namespace KIPIImgurPlugin
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
    setWindowIcon(QIcon::fromTheme(QStringLiteral("kipi-imgur")));
    setWindowTitle(i18n("Export to imgur.com"));
    setModal(false);

    startButton()->setText(i18n("Upload"));
    startButton()->setToolTip(i18n("Start upload to Imgur"));

    startButton()->setEnabled(!d->webService->imageQueue()->isEmpty());

    // ---------------------------------------------------------------
    // About data and help button.

    KPAboutData* const about = new KPAboutData(ki18n("Imgur Export"),
                                   0,
                                   KAboutLicense::GPL,
                                   ki18n("A tool to export images to Imgur web service"),
                                   ki18n("(c) 2012-2013, Marius Orcsik"));

    about->addAuthor(ki18n("Marius Orcsik").toString(),
                     ki18n("Author").toString(),
                     QStringLiteral("marius at habarnam dot ro"));

    about->addAuthor(ki18n("Gilles Caulier").toString(),
                     ki18n("Developer").toString(),
                     QStringLiteral("caulier dot gilles at gmail dot com"));

    about->setHandbookEntry(QStringLiteral("imgur"));
    setAboutData(about);

    // ------------------------------------------------------------

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotStartUpload()));

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    connect(d->webService, SIGNAL(signalQueueChanged()),
            this, SLOT(slotImageQueueChanged()));

    connect(d->webService, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(d->webService, SIGNAL(signalUploadStart(QUrl)),
            d->widget, SLOT(slotImageUploadStart(QUrl)));

    connect(d->webService, SIGNAL(signalError(QUrl,ImgurError)),
            d->widget, SLOT(slotImageUploadError(QUrl,ImgurError)));

    connect(d->webService, SIGNAL(signalSuccess(QUrl,ImgurSuccess)),
            d->widget, SLOT(slotImageUploadSuccess(QUrl,ImgurSuccess)));

    // this signal/slot controls if the webservice should continue upload or not
    connect(d->webService, SIGNAL(signalError(QUrl,ImgurError)),
            this, SLOT(slotAddPhotoError(QUrl,ImgurError)));

    connect(d->webService, SIGNAL(signalSuccess(QUrl,ImgurSuccess)),
            this, SLOT(slotAddPhotoSuccess(QUrl,ImgurSuccess)));

    connect(this, SIGNAL(signalContinueUpload(bool)),
            d->webService, SLOT(slotContinueUpload(bool)));

    // adding/removing items from the image list
    connect(d->widget, SIGNAL(signalAddItems(QList<QUrl>)),
            d->webService, SLOT(slotAddItems(QList<QUrl>)));

    connect(d->widget, SIGNAL(signalRemoveItems(QList<QUrl>)),
            d->webService, SLOT(slotRemoveItems(QList<QUrl>)));

   // ---------------------------------------------------------------
#ifdef OAUTH_ENABLED
    connect(d->widget, SIGNAL(signalClickedChangeUser()),
            d->webService, SLOT(slotOAuthLogin()));

    //connect(d->webService, SIGNAL(signalAuthenticated(bool)),
    //        d->widget, SLOT(slotAuthenticated(bool)));

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

void ImgurWindow::setContinueUpload(bool state)
{
    setRejectButtonMode(state ? QDialogButtonBox::Cancel : QDialogButtonBox::Close);

    emit signalContinueUpload(state);
}

void ImgurWindow::slotCancel()
{
    setContinueUpload(false);
    // Must cancel the transfer
    d->webService->cancel();
    d->webService->imageQueue()->clear();

    d->widget->imagesList()->cancelProcess();
    d->widget->progressBar()->setVisible(false);
    d->widget->progressBar()->progressCompleted();
}

void ImgurWindow::slotFinished()
{
    d->widget->imagesList()->listView()->clear();
    saveSettings();
}

void ImgurWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void ImgurWindow::slotStartUpload()
{
    setContinueUpload(true);
}

void ImgurWindow::reactivate()
{
    d->widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void ImgurWindow::slotImageQueueChanged()
{
    startButton()->setEnabled(!d->webService->imageQueue()->isEmpty());
}

void ImgurWindow::slotAddPhotoError(const QUrl& /*currentImage*/, const ImgurError& error)
{
    if (!d->webService->imageQueue()->isEmpty())
    {
        if (KMessageBox::warningContinueCancel(this,
                                               i18n("Failed to upload photo to Imgur: %1\n"
                                                    "Do you want to continue?", error.message))
            == KMessageBox::Continue)
        {
            setContinueUpload(true);
        }
        else
        {
            setContinueUpload(false);
        }

    }
    else
    {
        KMessageBox::sorry(this,i18n("Failed to upload photo to Imgur: %1\n", error.message));
    }

    return;
}

void ImgurWindow::slotAddPhotoSuccess(const QUrl& /*currentImage*/, const ImgurSuccess& /*success*/)
{
   setContinueUpload(true);
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
        startButton()->setEnabled(yes);
    }
    else if (KMessageBox::warningContinueCancel(this, err)
               == KMessageBox::Continue)
    {
        startButton()->setEnabled(true);
    }
}

void ImgurWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        startButton()->setEnabled(false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        startButton()->setEnabled(!d->webService->imageQueue()->isEmpty());
    }
}

void ImgurWindow::readSettings()
{
    KConfig config(QStringLiteral("kipirc"));
    //KConfigGroup group = config.group(QString("Imgur Settings"));

    KConfigGroup group2 = config.group("Imgur Dialog");
    KWindowConfig::restoreWindowSize(windowHandle(), group2);
}

void ImgurWindow::saveSettings()
{
    KConfig config(QStringLiteral("kipirc"));
    //KConfigGroup group = config.group(QString("Imgur Settings"));

    KConfigGroup group2 = config.group("Imgur Dialog");
    KWindowConfig::saveWindowSize(windowHandle(), group2);
    config.sync();
}

} // namespace KIPIImgurPlugin
