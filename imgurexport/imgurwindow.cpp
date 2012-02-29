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
#include <klocale.h>

// Local includes

#include "kpmetadata.h"
#include "kpimageinfo.h"

using namespace KExiv2Iface;

namespace KIPIImgurExportPlugin
{

ImgurWindow::ImgurWindow(Interface* const interface, QWidget* const parent)
    : KDialog(parent)
{

    m_widget     = new ImgurWidget(interface, this);
    m_webService = new ImgurTalker(interface, this);

    setMainWidget(m_widget);

    setWindowIcon(KIcon("imgur"));
    setWindowTitle(i18n("Export to imgur.com"));

    setDefaultButton(Close);
    setModal(false);

    connect(m_webService, SIGNAL(signalError(ImgurError)),
            this, SLOT(slotAddPhotoError(ImgurError)));

    connect(m_webService, SIGNAL(signalSuccess(ImgurSuccess)),
            this, SLOT(slotAddPhotoSuccess(ImgurSuccess)));

    connect(m_widget, SIGNAL(signalAddItems(KUrl::List)),
            m_webService, SLOT(slotAddItems(KUrl::List)));

    setButtons(KDialog::Close | KDialog::User1);

    setButtonGuiItem(KDialog::User1,
                     KGuiItem(i18n("Upload"), "network-workgroup",
                              i18n("Start upload to Imgur")));

    enableButton(KDialog::User1, !m_widget->imagesList()->imageUrls().isEmpty());

    connect(m_widget, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(m_webService, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

//    connect(this, SIGNAL(user1Clicked()),
//            this, SLOT(slotStartUpload()));

    connect(this, SIGNAL(buttonClicked(KDialog::ButtonCode)),
            this, SLOT(slotButtonClicked(KDialog::ButtonCode)));
}

ImgurWindow::~ImgurWindow()
{
    // TODO
}

void ImgurWindow::slotStartUpload() 
{
    kDebug() << "Start upload";

    m_widget->imagesList()->clearProcessedStatus();
    KUrl::List* m_transferQueue = m_webService->imageQueue();

    if (m_transferQueue->isEmpty())
    {
        kDebug() << "Upload queue empty. Exiting.";
        return;
    }

    m_imagesTotal = m_transferQueue->count();
    m_imagesCount = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(0);
    m_widget->progressBar()->setVisible(true);

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
            m_webService->cancel();

            m_widget->imagesList()->cancelProcess();
            m_widget->progressBar()->setVisible(false);

            // close the dialog
            m_widget->imagesList()->listView()->clear();
            done(Close);
            break;
        default:
            KDialog::slotButtonClicked(button);
            break;
    }
}

void ImgurWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void ImgurWindow::slotImageListChanged()
{
    enableButton(User1, !m_widget->imagesList()->imageUrls().isEmpty());
}

void ImgurWindow::slotAddPhotoError(ImgurError error)
{
    KUrl::List* m_transferQueue = m_webService->imageQueue();
    KUrl currentImage           = m_transferQueue->first();

    kError() << error.message;
    m_widget->imagesList()->processed(currentImage, false);

    if (KMessageBox::warningContinueCancel(this,
                                           i18n("Failed to upload photo to Imgur: %1\n"
                                                "Do you want to continue?", error.message))
        != KMessageBox::Continue)
    {
        m_widget->progressBar()->setVisible(false);
        m_transferQueue->clear();
        return;
    }

    uploadNextItem();
}

void ImgurWindow::slotAddPhotoSuccess(ImgurSuccess success)
{
    KUrl::List* m_transferQueue = m_webService->imageQueue();
    KUrl currentImage           = m_transferQueue->first();

    m_widget->imagesList()->processed(currentImage, true);

    m_webService->imageQueue()->pop_front();
    m_imagesCount++;

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
        enableButton(User1, m_webService->imageQueue()->isEmpty());
    }
}

void ImgurWindow::closeEvent(QCloseEvent* e)
{
    kDebug() << "Close event" << e;
}

void ImgurWindow::uploadNextItem()
{
    KUrl::List* m_transferQueue = m_webService->imageQueue();

    if (m_transferQueue->empty())
    {
        m_widget->progressBar()->hide();
        return;
    }

    KUrl current = m_transferQueue->first();
    m_widget->imagesList()->processing(current);

    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(m_imagesCount);

    kDebug() << "Starting upload for:" << current;
    m_webService->imageUpload(current);
}

} // namespace KIPIImgurExportPlugin
