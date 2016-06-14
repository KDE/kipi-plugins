/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
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

#include "imageshackwindow.h"

// Qt includes

#include <QWindow>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPointer>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>
#include <kwindowconfig.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "kipiplugins_debug.h"
#include "imageshack.h"
#include "imageshackwidget.h"
#include "imageshacktalker.h"
#include "kpaboutdata.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "kplogindialog.h"
#include "newalbumdlg.h"

namespace KIPIImageshackPlugin
{

ImageshackWindow::ImageshackWindow(QWidget* const parent, Imageshack* const imghack)
    : KPToolDialog(parent)
{
    m_imageshack = imghack;
    m_widget     = new ImageshackWidget(this, imghack, iface(), QString::fromLatin1("ImageShack"));
    m_widget->setMinimumSize(700, 500);
    setMainWidget(m_widget);
    setWindowTitle(i18n("Export to Imageshack"));
    setModal(true);

    m_albumDlg =  new NewAlbumDlg(this, QString::fromLatin1("ImageShack"));

    connect(m_widget->m_chgRegCodeBtn, SIGNAL(clicked(bool)),
            this, SLOT(slotChangeRegistrantionCode()));

    startButton()->setText(i18n("Upload"));
    startButton()->setToolTip(i18n("Start upload to Imageshack web service"));

    startButton()->setEnabled(false);

    connect(m_widget->m_imgList, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    // About data
    KPAboutData* const about = new KPAboutData(ki18n("Imageshack Export"),
                                   0,
                                   KAboutLicense::GPL,
                                   ki18n("A kipi plugin to export images to Imageshack web service."),
                                   ki18n("(c) 2012, Dodon Victor\n"));

    about->addAuthor(ki18n("Dodon Victor").toString(),
                     ki18n("Author").toString(),
                     QString::fromLatin1("dodonvictor at gmail dot com"));

    about->setHandbookEntry(QString::fromLatin1("imageshack"));
    setAboutData(about);

    // -----------------------------------------------------------

    connect(this, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    m_talker = new ImageshackTalker(imghack);

    connect(m_talker, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(m_talker, SIGNAL(signalJobInProgress(int,int,QString)),
            this, SLOT(slotJobInProgress(int,int,QString)));

    connect(m_talker, SIGNAL(signalLoginDone(int,QString)),
            this, SLOT(slotLoginDone(int,QString)));

    connect(m_talker, SIGNAL(signalGetGalleriesDone(int,QString)),
            this, SLOT(slotGetGalleriesDone(int,QString)));

    connect(m_talker, SIGNAL(signalUpdateGalleries(QStringList,QStringList)),
            m_widget, SLOT(slotGetGalleries(QStringList,QStringList)));

    connect(m_talker, SIGNAL(signalAddPhotoDone(int,QString)),
            this, SLOT(slotAddPhotoDone(int,QString)));

    connect(m_widget, SIGNAL(signalReloadGalleries()),
            this, SLOT(slotGetGalleries()));

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotStartTransfer()));

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancelClicked()));

    connect(m_widget->getNewAlbmBtn(),SIGNAL(clicked()),
            this,SLOT(slotNewAlbumRequest()));

    readSettings();

    QTimer::singleShot(20, this, SLOT(authenticate()));
}

ImageshackWindow::~ImageshackWindow()
{
}

void ImageshackWindow::slotImageListChanged()
{
    startButton()->setEnabled(!m_widget->m_imgList->imageUrls().isEmpty());
}

void ImageshackWindow::slotFinished()
{
    saveSettings();
    m_widget->m_progressBar->progressCompleted();
    m_widget->m_imgList->listView()->clear();
}

void ImageshackWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void ImageshackWindow::readSettings()
{
    winId();
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("Imageshack Settings");
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size());

    if (group.readEntry("Private", false))
    {
        m_widget->m_privateImagesChb->setChecked(true);
    }

    if (group.readEntry("Rembar", false))
    {
        m_widget->m_remBarChb->setChecked(true);
    }
    else
    {
        m_widget->m_remBarChb->setChecked(false);
    }
}

void ImageshackWindow::saveSettings()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("Imageshack Settings");
    KWindowConfig::saveWindowSize(windowHandle(), group);

    group.writeEntry("Private", m_widget->m_privateImagesChb->isChecked());
    group.writeEntry("Rembar", m_widget->m_remBarChb->isChecked());
    group.sync();
}

void ImageshackWindow::slotStartTransfer()
{
    m_widget->m_imgList->clearProcessedStatus();
    m_transferQueue = m_widget->m_imgList->imageUrls();

    if (m_transferQueue.isEmpty())
    {
        return;
    }

    qCDebug(KIPIPLUGINS_LOG) << "Transfer started!";

    m_imagesTotal = m_transferQueue.count();
    m_imagesCount = 0;

    m_widget->m_progressBar->setFormat(i18n("%v / %m"));
    m_widget->m_progressBar->setMaximum(m_imagesTotal);
    m_widget->m_progressBar->setValue(0);
    m_widget->m_progressBar->setVisible(true);
    m_widget->m_progressBar->progressScheduled(i18n("Image Shack Export"), false, true);
    m_widget->m_progressBar->progressThumbnailChanged(QIcon::fromTheme(QString::fromLatin1("kipi")).pixmap(22, 22));

    uploadNextItem();
}

void ImageshackWindow::slotCancelClicked()
{
    m_talker->cancel();
    m_transferQueue.clear();
    m_widget->m_imgList->cancelProcess();
    m_widget->m_progressBar->setVisible(false);
    m_widget->m_progressBar->progressCompleted();
}

void ImageshackWindow::slotChangeRegistrantionCode()
{
    qCDebug(KIPIPLUGINS_LOG) << "Change registration code";
    authenticate();
}

void ImageshackWindow::authenticate()
{
    emit signalBusy(true);
    m_widget->progressBar()->show();
    m_widget->m_progressBar->setValue(0);
    m_widget->m_progressBar->setMaximum(4);
    m_widget->progressBar()->setFormat(i18n("Authenticating..."));

    KIPIPlugins::KPLoginDialog* const dlg = new KIPIPlugins::KPLoginDialog(this, QString::fromLatin1("ImageShack"));

    if (dlg->exec() == QDialog::Accepted)
    {
        m_imageshack->setEmail(dlg->login());
        m_imageshack->setPassword(dlg->password());
        m_talker->authenticate();
    }
}

void ImageshackWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        m_widget->m_chgRegCodeBtn->setEnabled(false);
        startButton()->setEnabled(false);
        setRejectButtonMode(QDialogButtonBox::Cancel);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        m_widget->m_chgRegCodeBtn->setEnabled(true);
        startButton()->setEnabled(m_imageshack->loggedIn() &&
                                  !m_widget->imagesList()->imageUrls().isEmpty());
        setRejectButtonMode(QDialogButtonBox::Close);
    }
}

void ImageshackWindow::slotJobInProgress(int step, int maxStep, const QString &format)
{
    if (maxStep > 0)
    {
        m_widget->m_progressBar->setMaximum(maxStep);
    }
    m_widget->m_progressBar->setValue(step);

    if (!format.isEmpty())
    {
        m_widget->m_progressBar->setFormat(format);
    }
}

void ImageshackWindow::slotLoginDone(int errCode, const QString& errMsg)
{
    m_widget->updateLabels();

    if (!errCode && m_imageshack->loggedIn())
    {
        m_imageshack->saveSettings();
        startButton()->setEnabled(!m_widget->imagesList()->imageUrls().isEmpty());
        m_talker->getGalleries();
    }
    else
    {
        QMessageBox::critical(this, QString(), i18n("Login failed: %1\n", errMsg));
        startButton()->setEnabled(false);
        m_widget->m_progressBar->setVisible(false);
        slotBusy(false);
    }
}

void ImageshackWindow::slotGetGalleriesDone(int errCode, const QString &errMsg)
{
    slotBusy(false);
    m_widget->m_progressBar->setVisible(false);

    if (errCode)
    {
        QMessageBox::critical(this, QString(), i18n("Failed to get galleries list: %1\n", errMsg));
    }
}

void ImageshackWindow::uploadNextItem()
{
    if (m_transferQueue.empty())
    {
        m_widget->m_progressBar->hide();
        return;
    }

    m_widget->m_imgList->processing(m_transferQueue.first());
    QString imgPath = m_transferQueue.first().toLocalFile();

    m_widget->m_progressBar->setMaximum(m_imagesTotal);
    m_widget->m_progressBar->setValue(m_imagesCount);

    QMap<QString, QString> opts;

    if (m_widget->m_privateImagesChb->isChecked())
    {
        opts[QString::fromLatin1("public")] = QString::fromLatin1("no");
    }

    if (m_widget->m_remBarChb->isChecked())
    {
        opts[QString::fromLatin1("rembar")] = QString::fromLatin1("yes");
    }

    // tags
    if (!m_widget->m_tagsFld->text().isEmpty())
    {
        QString str = m_widget->m_tagsFld->text();
        QStringList tagsList;
        tagsList = str.split(QRegExp(QString::fromLatin1("\\W+")), QString::SkipEmptyParts);
        opts[QString::fromLatin1("tags")] = tagsList.join(QString::fromLatin1(","));
    }

    opts[QString::fromLatin1("auth_token")] = m_imageshack->authToken();

    int gidx = m_widget->m_galleriesCob->currentIndex();

    qCDebug(KIPIPLUGINS_LOG) << "Album ID is "<< m_widget->m_galleriesCob->itemData(gidx).toString();

    switch(gidx)
    {
        case 0:
            m_talker->uploadItem(imgPath, opts);
            break;
        case 1:
            opts[QString::fromLatin1("album")] = m_newAlbmTitle;
            m_talker->uploadItemToGallery(imgPath, m_newAlbmTitle, opts);
            break;
        default:
            opts[QString::fromLatin1("album")] = m_widget->m_galleriesCob->itemData(gidx).toString();
            m_talker->uploadItemToGallery(imgPath, m_widget->m_galleriesCob->itemData(gidx).toString(), opts);
    }
}

void ImageshackWindow::slotAddPhotoDone(int errCode, const QString& errMsg)
{
    m_widget->m_imgList->processed(m_transferQueue.first(), (errCode == 0));

    if (!errCode)
    {
        m_widget->imagesList()->removeItemByUrl(m_transferQueue.first());
        m_transferQueue.pop_front();
        m_imagesCount++;
    }
    else
    {
        if (QMessageBox::question(this, i18n("Uploading Failed"),
                                  i18n("Failed to upload photo into Imageshack: %1\n"
                                       "Do you want to continue?", errMsg))
            != QMessageBox::Yes)
        {
            m_widget->m_progressBar->setVisible(false);
            m_transferQueue.clear();
            return;
        }
    }

    uploadNextItem();
}

void ImageshackWindow::slotGetGalleries()
{
    m_widget->m_progressBar->setVisible(true);
    m_talker->getGalleries();
}

void ImageshackWindow::slotNewAlbumRequest()
{
    if (m_albumDlg->exec() == QDialog::Accepted)
    {
        m_newAlbmTitle = m_albumDlg->getAlbumTitle();
    }
}

} // namespace KIPIImageshackPlugin
