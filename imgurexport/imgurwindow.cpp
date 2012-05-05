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

// Local includes

#include "kpmetadata.h"
#include "kpimageinfo.h"
#include "kpaboutdata.h"
#include "kpversion.h"

namespace KIPIImgurExportPlugin
{

class ImgurWindow::ImgurWindowPriv
{
public:

    ImgurWindowPriv()
    {
        imagesCount = 0;
        imagesTotal = 0;
        webService  = 0;
        widget      = 0;
    }

    int          imagesCount;
    int          imagesTotal;

    ImgurTalker* webService;
    ImgurWidget* widget;
};

ImgurWindow::ImgurWindow(QWidget* const /*parent*/)
    : KPToolDialog(0), d(new ImgurWindowPriv)
{
    d->widget     = new ImgurWidget(this);
    d->webService = new ImgurTalker(iface(), this);

    setMainWidget(d->widget);
    setWindowIcon(KIcon("imgur"));
    setWindowTitle(i18n("Export to imgur.com"));
    setModal(false);

    setButtons(Help | Close | User1);
    setButtonGuiItem(User1, KGuiItem(i18n("Upload"), "network-workgroup", i18n("Start upload to Imgur")));
    setDefaultButton(Close);

    connect(d->webService, SIGNAL(signalError(ImgurError)),
            this, SLOT(slotAddPhotoError(ImgurError)));

    connect(d->webService, SIGNAL(signalSuccess(ImgurSuccess)),
            this, SLOT(slotAddPhotoSuccess(ImgurSuccess)));

    connect(d->widget, SIGNAL(signalAddItems(KUrl::List)),
            d->webService, SLOT(slotAddItems(KUrl::List)));

    enableButton(User1, !d->widget->imagesList()->imageUrls().isEmpty());

    // ---------------------------------------------------------------
    // About data and help button.

    KPAboutData* about = new KPAboutData(ki18n("Imgur Export"),
                             0,
                             KAboutData::License_GPL,
                             ki18n("A tool to export images to Imgur web service"),
                             ki18n("(c) 2012, Marius Orcsik"));

    about->addAuthor(ki18n("Marius Orcsik"), ki18n("Author and Maintainer"),
                     "marius at habarnam dot ro");

    about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->setHandbookEntry("imgurexport");
    setAboutData(about);

    // ------------------------------------------------------------

    connect(d->widget, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(d->webService, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(this, SIGNAL(buttonClicked(KDialog::ButtonCode)),
            this, SLOT(slotButtonClicked(KDialog::ButtonCode)));

//    connect(this, SIGNAL(user1Clicked()),
//            this, SLOT(slotStartUpload()));
}

ImgurWindow::~ImgurWindow()
{
    delete d;
}

void ImgurWindow::slotStartUpload() 
{
    kDebug() << "Start upload";

    d->widget->imagesList()->clearProcessedStatus();
    KUrl::List* m_transferQueue = d->webService->imageQueue();

    if (m_transferQueue->isEmpty())
    {
        kDebug() << "Upload queue empty. Exiting.";
        return;
    }

    d->imagesTotal = m_transferQueue->count();
    d->imagesCount = 0;

    d->widget->progressBar()->setFormat(i18n("%v / %m"));
    d->widget->progressBar()->progressScheduled(i18n("Export to Imgur"), true, true);
    d->widget->progressBar()->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));
    d->widget->progressBar()->setMaximum(d->imagesTotal);
    d->widget->progressBar()->setValue(0);
    d->widget->progressBar()->setVisible(true);

    kDebug() << "Upload queue has " << m_transferQueue->length() << "items";

    uploadNextItem();
}

void ImgurWindow::slotButtonClicked(int button)
{
    kDebug() << "Button clicked" << button;

    switch (button)
    {
        case KDialog::User1:
            slotStartUpload();
            break;
        case KDialog::Close:
            // Must cancel the transfer
            d->webService->cancel();

            d->widget->imagesList()->cancelProcess();
            d->widget->progressBar()->setVisible(false);
            d->widget->progressBar()->progressCompleted();

            // close the dialog
            d->widget->imagesList()->listView()->clear();
            done(Close);
            break;
        default:
            KDialog::slotButtonClicked(button);
            break;
    }
}

void ImgurWindow::reactivate()
{
    d->widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void ImgurWindow::slotImageListChanged()
{
    enableButton(User1, !d->widget->imagesList()->imageUrls().isEmpty());
}

void ImgurWindow::slotAddPhotoError(ImgurError error)
{
    KUrl::List* m_transferQueue = d->webService->imageQueue();
    KUrl currentImage           = m_transferQueue->first();

    kError() << error.message;
    d->widget->imagesList()->processed(currentImage, false);

    if (KMessageBox::warningContinueCancel(this,
                                           i18n("Failed to upload photo to Imgur: %1\n"
                                                "Do you want to continue?", error.message))
        != KMessageBox::Continue)
    {
        d->widget->progressBar()->setVisible(false);
        d->widget->progressBar()->progressCompleted();
        m_transferQueue->clear();
        return;
    }

    uploadNextItem();
}

void ImgurWindow::slotAddPhotoSuccess(ImgurSuccess success)
{
    KUrl::List* m_transferQueue = d->webService->imageQueue();
    KUrl currentImage           = m_transferQueue->first();

    d->widget->imagesList()->processed(currentImage, true);

    d->webService->imageQueue()->pop_front();
    d->imagesCount++;

    const QString sUrl       = success.links.imgur_page.toEncoded();
    const QString sDeleteUrl = success.links.delete_page.toEncoded();

    const QString path       = currentImage.toLocalFile();

    // we add tags to the image
    KPMetadata meta(path);
    meta.setXmpTagString("Xmp.kipi.ImgurURL", sUrl);
    meta.setXmpTagString("Xmp.kipi.ImgurDeleteURL", sDeleteUrl);
    bool saved = meta.applyChanges();

    kDebug() << "Metadata" << (saved ? "Saved" : "Not Saved") << "to" << path;
    kDebug () << "URL" << sUrl;
    kDebug () << "Delete URL" << sDeleteUrl;

    uploadNextItem();
}

void ImgurWindow::slotAddPhotoDone()
{
    // NOTE: not used atm
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
        enableButton(User1, d->webService->imageQueue()->isEmpty());
    }
}

void ImgurWindow::closeEvent(QCloseEvent* e)
{
    kDebug() << "Close event" << e;
}

void ImgurWindow::uploadNextItem()
{
    KUrl::List* m_transferQueue = d->webService->imageQueue();

    if (m_transferQueue->empty())
    {
        d->widget->progressBar()->hide();
        d->widget->progressBar()->progressCompleted();
        return;
    }

    KUrl current = m_transferQueue->first();
    d->widget->imagesList()->processing(current);

    d->widget->progressBar()->setMaximum(d->imagesTotal);
    d->widget->progressBar()->setValue(d->imagesCount);
    d->widget->progressBar()->progressStatusChanged(i18n("Processing %1", current.fileName()));

    kDebug() << "Starting upload for:" << current;
    d->webService->imageUpload(current);
}

} // namespace KIPIImgurExportPlugin
