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

// local
#include "imgurwindow.h"

using namespace KIPIPlugins;

namespace KIPIImgurExportPlugin
{

ImgurWindow::ImgurWindow(KIPI::Interface* interface, QWidget* parent)
    : KDialog(parent)
{

    m_widget = new ImgurWidget(interface, this);
    m_webService = new ImgurTalker(interface, this);

    setMainWidget(m_widget);

    setWindowIcon(KIcon("imgur"));
    setWindowTitle(i18n("Export to the imgur.com web service"));

    setDefaultButton(Close);
    setModal(false);

//    show();
//    connect(m_webService, SIGNAL(signalUploadStart(KUrl)),
//            m_widget, SLOT(processing(KUrl)));

//    connect(m_webService, SIGNAL(signalUploadDone(KUrl, bool)),
//            m_widget, SLOT(processed(KUrl, bool)));

//    connect(m_webService, SIGNAL(signalUploadProgress(int)),
//            m_widget, SLOT(slotProgressTimerDone(int)));

//    connect(m_widget, SIGNAL(signalAddItems(KUrl::List)),
//            m_webService, SLOT(slotAddItems(KUrl::List)));

//    m_webService->startUpload();

    setButtons(KDialog::Close | KDialog::User1 | KDialog::Help);
    setButtonGuiItem(User1,
                     KGuiItem(i18n("Upload"), "network-workgroup",
                              i18n("Start upload to Imageshack web service")));
    enableButton(User1, !m_widget->imagesList()->imageUrls().isEmpty());

//    connect(m_widget->m_imgagesList, SIGNAL(signalImageListChanged()),
//            this, SLOT(slotImageListChanged()));

//    connect(this, SIGNAL(user1Clicked()),
//            this, SLOT(slotStartUpload()));
}

ImgurWindow::~ImgurWindow()
{
    //
}

void ImgurWindow::slotStartUpload () {
//    m_widget->imagesList()->clearProcessedStatus();
//    KUrl::List m_transferQueue = m_widget->imagesList()->imageUrls();

//    if (m_transferQueue.isEmpty())
//    {
//        return;
//    }

//    int m_imagesTotal = m_webService->processQueue().count();
//    int m_imagesCount = 0;

//    m_widget->progressBar()->setFormat(i18n("%v / %m"));
//    m_widget->progressBar()->setMaximum(m_imagesTotal);
//    m_widget->progressBar()->setValue(0);
//    m_widget->progressBar()->setVisible(true);

////    uploadNextItem();
//    m_webService->startUpload();
}

void ImgurWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

//void ImgurWindow::slotHelp()
//{
////    KToolInvocation::invokeHelp("imageshackexport", "kipi-plugins");
//}

void ImgurWindow::slotImageListChanged()
{
//    enableButton(User1, !m_widget->m_imgList->imageUrls().isEmpty());
}

void ImgurWindow::slotButtonClicked(int button)
{
//    switch (button)
//    {
//        case Close:
//            if (m_widget->progressBar()->isVisible())
//            {
//                // Must cancel the transfer
////                m_talker->cancel();
////                m_transferQueue.clear();
////                m_widget->m_imgList->cancelProcess();
////                m_widget->m_progressBar->setVisible(false);
//            }
//            else
//            {
//                // close the dialog
////                saveSettings();
////                m_widget->m_imgList->listView()->clear();
////                done(Close);
//            }
//            break;
//        case User1:
////            slotStartTransfer();
//            break;
//        default:
////            KDialog::slotButtonClicked(button);
//    }
}

void ImgurWindow::slotAddPhotoDone(int errCode, const QString& errMsg)
{
//    kDebug() << errCode << "----------------------++++";
//    m_widget->m_imgList->processed(m_transferQueue.first(), (errCode == 0));

//    if (!errCode)
//    {
//        m_transferQueue.pop_front();
//        m_imagesCount++;
//    }
//    else
//    {
//        if (KMessageBox::warningContinueCancel(this,
//                                               i18n("Failed to upload photo to Imageshack: %1\n"
//                                                    "Do you want to continue?", errMsg))
//            != KMessageBox::Continue)
//        {
//            m_widget->m_progressBar->setVisible(false);
//            m_transferQueue.clear();
//            return;
//        }
//    }

//    uploadNextItem();
}

void ImgurWindow::slotBusy(bool val)
{
//    if (val)
//    {
//        setCursor(Qt::WaitCursor);
//        m_widget->m_chgRegCodeBtn->setEnabled(false);
//        enableButton(User1, false);
//    }
//    else
//    {
//        setCursor(Qt::ArrowCursor);
//        m_widget->m_chgRegCodeBtn->setEnabled(true);
//        enableButton(User1, m_widget->imagesList()->imageUrls().isEmpty());
//    }
}

void ImgurWindow::closeEvent(QCloseEvent* e)
{
}

void ImgurWindow::uploadNextItem()
{
//    if (m_transferQueue.empty())
//    {
//        m_widget->m_progressBar->hide();
//        return;
//    }

//    m_widget->m_imgList->processing(m_transferQueue.first());
//    QString imgPath = m_transferQueue.first().toLocalFile();

//    m_widget->m_progressBar->setMaximum(m_imagesTotal);
//    m_widget->m_progressBar->setValue(m_imagesCount);

//    QMap<QString, QString> opts;

//    if (m_widget->m_privateImagesChb->isChecked())
//    {
//        opts["public"] = "no";
//    }

//    if (m_widget->m_remBarChb->isChecked())
//    {
//        opts["rembar"] = "yes";
//    }

//    if (m_widget->m_predefSizeRdb->isChecked())
//    {
//        opts["optimage"] = "1";
//        opts["optsize"] = m_widget->m_resizeOptsCob->itemData(m_widget->m_resizeOptsCob->currentIndex()).toString();
//    }
//    else if (m_widget->m_customSizeRdb->isChecked())
//    {
//        opts["optimage"] = "1";
//        QString dim =  "";
//        dim.append(QString("%1x%2").arg(m_widget->m_widthSpb->value()).arg(m_widget->m_heightSpb->value()));
//        opts["optsize"] = dim;
//    }

//    // tags
//    if (!m_widget->m_tagsFld->text().isEmpty())
//    {
//        QString str = m_widget->m_tagsFld->text();
//        QStringList tagsList;
//        tagsList = str.split(QRegExp("\\W+"), QString::SkipEmptyParts);
//        opts["tags"] = tagsList.join(",");
//    }

//    opts["cookie"] = m_imageshack->registrationCode();

//    m_talker->uploadItem(imgPath, opts);
}

} // namespace KIPIImgurExportPlugin
