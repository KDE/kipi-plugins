/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2012-2012 by Marius Orcsik <marius at habarnam dot ro>
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

// KDE includes

#include <kdebug.h>
#include <kmessagebox.h>

// Local includes

#include "kpmetadata.h"
#include "kpimageinfo.h"

using namespace KIPIPlugins;
using namespace KExiv2Iface;

namespace KIPIImgurExportPlugin
{

ImgurWindow::ImgurWindow(KIPI::Interface* interface, QWidget* parent)
    : KDialog(parent)
{

    m_widget = new ImgurWidget(interface, this);
    m_webService = new ImgurTalker(interface, this);

    setMainWidget(m_widget);

    setWindowIcon(KIcon("imgur"));
    setWindowTitle(i18n("Export to imgur.com"));

    setDefaultButton(Close);
    setModal(false);

//    connect(m_webService, SIGNAL(signalUploadStart(KUrl)),
//            m_widget, SLOT(processing(KUrl)));

    connect(m_webService, SIGNAL(signalUploadDone()),
            this, SLOT(slotAddPhotoDone()));

//    connect(m_webService, SIGNAL(signalUploadProgress(int)),
//            this, SLOT(slotProgressTimerDone(int)));

    connect(m_widget, SIGNAL(signalAddItems(KUrl::List)),
            m_webService, SLOT(slotAddItems(KUrl::List)));

    setButtons(KDialog::Close | KDialog::User1);

    setButtonGuiItem(KDialog::User1,
                     KGuiItem(i18n("Upload"), "network-workgroup",
                              i18n("Start upload to Imgur")));

    enableButton(KDialog::User1, !m_widget->imagesList()->imageUrls().isEmpty());

    connect(m_widget, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

//    connect(this, SIGNAL(user1Clicked()),
//            this, SLOT(slotStartUpload()));

    connect(this, SIGNAL(buttonClicked(KDialog::ButtonCode)),
            this, SLOT(slotButtonClicked(KDialog::ButtonCode)));
}

ImgurWindow::~ImgurWindow()
{
    //
}

void ImgurWindow::slotStartUpload() {
    kDebug() << "Start upload";

    m_widget->imagesList()->clearProcessedStatus();
    KUrl::List *m_transferQueue = m_webService->imageQueue();

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

void ImgurWindow::slotButtonClicked (int button)
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
        default:
            KDialog::slotButtonClicked(button);
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

void ImgurWindow::slotAddPhotoDone()
{
    KUrl::List *m_transferQueue = m_webService->imageQueue();

    ImgurError error = m_webService->error();
    ImgurSuccess success = m_webService->success();

    KUrl currentImage = m_transferQueue->first();

    QString errMsg = QString(error.message);

    kDebug() << error.message << "----------------------++++";
    m_widget->imagesList()->processed(currentImage, errMsg.isEmpty());

    if (errMsg.isEmpty())
    {
        //kDebug() << m_widget->imagesList()->imageUrls().length();
        m_webService->imageQueue()->pop_front();
        //kDebug() << m_widget->imagesList()->imageUrls().length();
        m_imagesCount++;

        QByteArray sUrl = success.links.imgur_page.toEncoded();
        QByteArray sDeleteUrl = success.links.delete_page.toEncoded();

        kDebug() << sUrl;

        const QString path = currentImage.toLocalFile();
//        KMessageBox::questionYesNo(this, i18n("Url: %1", sUrl));
        // we add tags to the image
        KPMetadata *meta = new KPMetadata(path);
//        meta.load(path);

        meta->setXmpTagString("Xmp.kipi.Imgur_URL", sUrl);
        meta->setXmpTagString("Xmp.kipi.Imgur_DeleteURL", sDeleteUrl);
//        meta.setXmpTagString("Xmp.BaseURL", sDeleteUrl);

        bool saved = meta->applyChanges();

        kDebug() << "Metadata" << (saved ? "Saved" : "Not Saved") << "to" << path << "\nURL" << meta->getXmp();
    }
    else
    {
        if (KMessageBox::warningContinueCancel(this,
                                               i18n("Failed to upload photo to Imgur: %1\n"
                                                    "Do you want to continue?", error.message))
            != KMessageBox::Continue)
        {
            m_widget->progressBar()->setVisible(false);
            m_transferQueue->clear();
            return;
        }
    }

    uploadNextItem();
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
        enableButton(User1, m_widget->imagesList()->imageUrls().isEmpty());
    }
}

void ImgurWindow::closeEvent(QCloseEvent* e)
{
    kDebug() << "Close event" << e;
}

void ImgurWindow::uploadNextItem()
{
    KUrl::List *m_transferQueue = m_webService->imageQueue();

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
