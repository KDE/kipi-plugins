/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009      by Timothée Groleau <kde at timotheegroleau dot com>
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

#include "swwindow.moc"

// Qt includes

#include <QFileInfo>
#include <QSpinBox>
#include <QCheckBox>
#include <QCloseEvent>

// KDE includes

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmenu.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kmessagebox.h>
#include <kprogressdialog.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpimageslist.h"
#include "kpaboutdata.h"
#include "kpmetadata.h"
#include "kpversion.h"
#include "switem.h"
#include "swconnector.h"
#include "swwidget.h"
#include "swalbum.h"
#include "swlogin.h"

using namespace KDcrawIface;

namespace KIPIShwupPlugin
{

SwWindow::SwWindow(const QString& tmpFolder, QWidget* const parent)
    : KPToolDialog(parent)
{
    m_tmpPath.clear();
    m_tmpDir      = tmpFolder;
    m_imagesCount = 0;
    m_imagesTotal = 0;
    m_widget      = new SwWidget(this, iface());

    setMainWidget(m_widget);
    setWindowIcon(KIcon("kipi-shwup"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);

    setWindowTitle(i18n("Export to Shwup Web Service"));
    setButtonGuiItem(User1, KGuiItem(i18n("Start Upload"), "network-workgroup",
                            i18n("Start upload to Shwup web service")));
     m_widget->setMinimumSize(700, 500);

    connect(m_widget->m_imgList, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()) );

    connect(m_widget->m_changeUserBtn, SIGNAL(clicked()),
            this, SLOT(slotUserChangeRequest()) );

    connect(m_widget->m_newAlbumBtn, SIGNAL(clicked()),
            this, SLOT(slotNewAlbumRequest()) );

    connect(m_widget, SIGNAL(reloadAlbums()),
            this, SLOT(slotReloadAlbumsRequest()) );

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()) );

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()) );

    // ------------------------------------------------------------------------

    KPAboutData* const about = new KPAboutData(ki18n("Shwup Export"), 0,
                                   KAboutData::License_GPL,
                                   ki18n("A Kipi plugin to export images "
                                         "to Shwup web service."),
                                   ki18n("(c) 2009, Timothée Groleau"));

    about->addAuthor(ki18n("Timothée Groleau"), ki18n("Author and maintainer"),
                     "kde at timotheegroleau dot com");

    about->setHandbookEntry("shwup");
    setAboutData(about);

    // ------------------------------------------------------------------------

    m_loginDlg  = new SwLogin(this);
    m_albumDlg  = new SwNewAlbum(this);

    // ------------------------------------------------------------------------

    m_connector = new SwConnector(this);

    connect(m_connector, SIGNAL(signalShwupKipiBlackListed()),
            this, SLOT(slotShwupKipiBlackListed()),
            Qt::QueuedConnection);

    connect(m_connector, SIGNAL(signalShwupSignatureError()),
            this, SLOT(slotShwupSignatureError()),
            Qt::QueuedConnection);

    connect(m_connector, SIGNAL(signalShwupInvalidCredentials()),
            this, SLOT(slotShwupInvalidCredentials()),
            Qt::QueuedConnection);

    connect(m_connector, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(m_connector, SIGNAL(signalRequestRestURLDone(int,QString)),
            this, SLOT(slotRequestRestURLDone(int,QString)));

    connect(m_connector, SIGNAL(signalListAlbumsDone(int,QString,QList<SwAlbum>)),
            this, SLOT(slotListAlbumsDone(int,QString,QList<SwAlbum>)));

    connect(m_connector, SIGNAL(signalCreateAlbumDone(int,QString,SwAlbum)),
            this, SLOT(slotCreateAlbumDone(int,QString,SwAlbum)));

    connect(m_connector, SIGNAL(signalAddPhotoDone(int,QString)),
            this, SLOT(slotAddPhotoDone(int,QString)));

    // ------------------------------------------------------------------------

    readSettings();

    buttonStateChange(false);

    m_connector->getRestServiceURL();
}

SwWindow::~SwWindow()
{
    delete m_connector;
}

void SwWindow::slotClose()
{
    writeSettings();
    m_widget->imagesList()->listView()->clear();
    done(Close);
}

void SwWindow::closeEvent(QCloseEvent *e)
{
    if (!e) return;

    writeSettings();
    m_widget->imagesList()->listView()->clear();
    e->accept();
}

void SwWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void SwWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("Shwup Settings");

    SwUser user;
    user.email       = grp.readEntry("User Email", "");
    user.password    = grp.readEntry("User Password", "");
    m_connector->setUser(user);

    m_currentAlbumID = grp.readEntry("Current Album", -1LL);

    if (grp.readEntry("Resize", false))
    {
        m_widget->m_resizeChB->setChecked(true);
        m_widget->m_dimensionSpB->setEnabled(true);
        m_widget->m_imageQualitySpB->setEnabled(true);
    }
    else
    {
        m_widget->m_resizeChB->setChecked(false);
        m_widget->m_dimensionSpB->setEnabled(false);
        m_widget->m_imageQualitySpB->setEnabled(false);
    }

    m_widget->m_dimensionSpB->setValue(grp.readEntry("Maximum Width",    1600));
    m_widget->m_imageQualitySpB->setValue(grp.readEntry("Image Quality", 90));

    KConfigGroup dialogGroup = config.group("Shwup Export Dialog");
    restoreDialogSize(dialogGroup);
}

void SwWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("Shwup Settings");
    SwUser user      = m_connector->getUser();

    grp.writeEntry("User Email",    user.email);
    grp.writeEntry("user Password", user.password);
    grp.writeEntry("Current Album", m_currentAlbumID);
    grp.writeEntry("Resize",        m_widget->m_resizeChB->isChecked());
    grp.writeEntry("Maximum Width", m_widget->m_dimensionSpB->value());
    grp.writeEntry("Image Quality", m_widget->m_imageQualitySpB->value());

    KConfigGroup dialogGroup = config.group("Shwup Export Dialog");
    saveDialogSize(dialogGroup);

    config.sync();
}

void SwWindow::slotRequestRestURLDone(int errCode, const QString& /*errMessage*/)
{
    if (errCode == 0)
    {
        authenticate();
    }
    else
    {
        // unable to contact service
        KMessageBox::error(this, i18n("The shwup.com service does not seem to be available at this time, please try again later."));
    }
}

void SwWindow::authenticate()
{
    buttonStateChange(false);

    SwUser user = m_connector->getUser();

    m_loginDlg->m_emailEdt->setText(user.email);
    m_loginDlg->m_passwordEdt->setText(user.password);

    if (m_loginDlg->exec() == QDialog::Accepted)
    {
        m_connector->logout();
        SwUser newUser;
        m_loginDlg->getUserProperties(newUser);
        m_connector->setUser(newUser);
        m_connector->listAlbums();
        m_widget->updateLabels(newUser.email, "");
    }
    else if (m_connector->isLoggedIn())
    {
        buttonStateChange(true);
    }
}

void SwWindow::slotShwupKipiBlackListed()
{
    KMessageBox::error(this, i18n("This application has been blacklisted by the shwup.com service."));
}

void SwWindow::slotShwupSignatureError()
{
}

void SwWindow::slotShwupInvalidCredentials()
{
    authenticate();
}

void SwWindow::slotListAlbumsDone(int errCode, const QString& errMsg, const QList <SwAlbum>& albumsList)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Shwup Call Failed: %1\n", errMsg));
        return;
    }

    m_albumsList = albumsList;
    m_widget->m_albumsCoB->clear();
    // m_widget->m_albumsCoB->addItem(i18n("<auto create>"), 0); // we don't expose such option for shwup yet...

    for (int i = 0; i < albumsList.size(); ++i)
    {
        m_widget->m_albumsCoB->addItem(KIcon("system-users"), albumsList.at(i).title,
                                       albumsList.at(i).id);
       if (m_currentAlbumID == albumsList.at(i).id)
       {
           m_widget->m_albumsCoB->setCurrentIndex(i);
       }
    }

    // if the call was successful, then it means the authentication credentials are valid, 
    // it means we can just enable the UI
    buttonStateChange(true); 
}

void SwWindow::buttonStateChange(bool state)
{
    m_widget->m_newAlbumBtn->setEnabled(state);
    m_widget->m_reloadAlbumsBtn->setEnabled(state);
    m_widget->m_albumsCoB->setEnabled(state);
    enableButton(User1, state);
}

void SwWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        m_widget->m_changeUserBtn->setEnabled(false);
        buttonStateChange(false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        m_widget->m_changeUserBtn->setEnabled(true);
        buttonStateChange(true);
    }
}

void SwWindow::slotUserChangeRequest()
{
    kDebug() << "Slot Change User Request";
    authenticate();
}

void SwWindow::slotReloadAlbumsRequest()
{
    kDebug() << "Reload Albums Request";
    m_connector->listAlbums();
}

void SwWindow::slotNewAlbumRequest()
{
    kDebug() << "Slot New Album Request";

    if (m_albumDlg->exec() == QDialog::Accepted)
    {
        kDebug() << "Calling New Album method";
        SwAlbum newAlbum;
        m_albumDlg->getAlbumProperties(newAlbum);
        m_connector->createAlbum(newAlbum);
    }
}

void SwWindow::slotLoginInvalid()
{
    m_loginDlg->hide();
    authenticate();
}

void SwWindow::slotStartTransfer()
{
    kDebug() << "slotStartTransfer invoked";

    m_transferQueue = m_widget->m_imgList->imageUrls();

    // we will not upload directories -- duh!
    for (int idx=m_transferQueue.count(); idx-->0; )
    {
        if ( QFileInfo( m_transferQueue.at(idx).path() ).isDir() )
        {
            m_widget->m_imgList->removeItemByUrl( m_transferQueue.at(idx) );
            m_transferQueue.removeAt(idx);
        }
    }

    if (m_transferQueue.isEmpty())
        return;

    m_currentAlbumID = m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toLongLong();
    m_imagesTotal    = m_transferQueue.count();
    m_imagesCount    = 0;

    m_progressDlg    = new KProgressDialog(this, i18n("Transfer Progress"));
    m_progressDlg->setMinimumDuration(0);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);
    m_progressDlg->progressBar()->setFormat(i18n("%v / %m"));

    connect(m_progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotTransferCancel()));

    uploadNextPhoto();
}

QString SwWindow::getImageCaption(const KPMetadata& meta) const
{
    QString caption = meta.getCommentsDecoded();

    if (!caption.isEmpty())
        return caption;

    if (meta.hasExif())
    {
        caption = meta.getExifComment();
        if (!caption.isEmpty())
            return caption;
    }

    if (meta.hasXmp())
    {
        caption = meta.getXmpTagStringLangAlt("Xmp.dc.description", QString(), false);
        if (!caption.isEmpty())
            return caption;

        caption = meta.getXmpTagStringLangAlt("Xmp.exif.UserComment", QString(), false);
        if (!caption.isEmpty())
            return caption;

        caption = meta.getXmpTagStringLangAlt("Xmp.tiff.ImageDescription", QString(), false);
        if (!caption.isEmpty())
            return caption;
    }

    if (meta.hasIptc())
    {
        caption = meta.getIptcTagString("Iptc.Application2.Caption", false);
        if (!caption.isEmpty() && !caption.trimmed().isEmpty())
            return caption;
    }

    return caption;
}

bool SwWindow::prepareImageForUpload(const QString& imgPath, bool isRAW, QString& caption)
{
    QImage image;

    if (isRAW)
    {
        kDebug() << "Get RAW preview " << imgPath;
        KDcraw::loadRawPreview(image, imgPath);
    }
    else
    {
        image.load(imgPath);
    }

    if (image.isNull())
        return false;

    // get temporary file name
    m_tmpPath = m_tmpDir + QFileInfo(imgPath).baseName().trimmed() + QString(".jpg");

    // rescale image if requested
    int maxDim = m_widget->m_dimensionSpB->value();

    if (m_widget->m_resizeChB->isChecked() && (image.width() > maxDim || image.height() > maxDim))
    {
        kDebug() << "Resizing to " << maxDim;
        image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
    }

    kDebug() << "Saving to temp file: " << m_tmpPath;
    image.save(m_tmpPath, "JPEG", m_widget->m_imageQualitySpB->value());

    // copy meta data to temporary image
    KPMetadata meta;

    if (meta.load(imgPath))
    {
        caption = getImageCaption(meta);
        meta.setImageDimensions(image.size());
        meta.setImageProgramId("Kipi-plugins", kipiplugins_version);
        meta.save(m_tmpPath);
    }
    else
    {
        caption.clear();
    }

    return true;
}

void SwWindow::uploadNextPhoto()
{
    if (m_transferQueue.isEmpty())
    {
        // done!
        m_progressDlg->hide();

        if (m_imagesTotal > 0)
        {
            SwAlbum album = m_albumsList.at( m_widget->m_albumsCoB->currentIndex() );
            KMessageBox::information(this,
                i18n("Upload complete. Visit \"<a href=\"%1\">%2</a>\" to view the album online and invite people.",
                    album.albumUrl,
                    album.title),
                i18n("Upload complete"),
                QString(),
                KMessageBox::AllowLink
            );
        }

        return;
    }

    m_progressDlg->progressBar()->setMaximum(m_imagesTotal);
    m_progressDlg->progressBar()->setValue(m_imagesCount);

    QString imgPath = m_transferQueue.first().path();
    QString caption;
    bool    res;

    // check if we have to RAW file -> use preview image then
    bool isRAW = KPMetadata::isRawFile(imgPath);

    if (isRAW || m_widget->m_resizeChB->isChecked())
    {
        if (!prepareImageForUpload(imgPath, isRAW, caption))
        {
            slotAddPhotoDone(666, i18n("Cannot open file"));
            return;
        }
        res = m_connector->addPhoto(m_tmpPath, m_currentAlbumID, caption);
    }
    else
    {
        KPMetadata meta;

        if (meta.load(imgPath))
            caption = getImageCaption(meta);
        else
            caption.clear();

        m_tmpPath.clear();
        res = m_connector->addPhoto(imgPath, m_currentAlbumID, caption);
    }

    if (!res)
    {
        slotAddPhotoDone(666, i18n("Cannot open file"));
        return;
    }

    m_progressDlg->setLabelText(i18n("Uploading file %1", m_transferQueue.first().path()));
}

void SwWindow::slotAddPhotoDone(int errCode, const QString& errMsg)
{
    // Remove temporary file if it was used
    if (!m_tmpPath.isEmpty())
    {
        QFile::remove(m_tmpPath);
        m_tmpPath.clear();
    }

    // Remove photo uploaded from the list
    m_widget->m_imgList->removeItemByUrl(m_transferQueue.first());
    m_transferQueue.pop_front();

    if (errCode == 0)
    {
        m_imagesCount++;
    }
    else
    {
        m_imagesTotal--;

        if (KMessageBox::warningContinueCancel(this,
                         i18n("Failed to upload photo into Shwup: %1\n"
                              "Do you want to continue?", errMsg))
                         != KMessageBox::Continue)
        {
            m_transferQueue.clear();
            m_progressDlg->hide();
            return;
        }
    }

    uploadNextPhoto();
}

void SwWindow::slotTransferCancel()
{
    m_transferQueue.clear();
    m_progressDlg->hide();
    m_connector->cancel();
}

void SwWindow::slotCreateAlbumDone(int errCode, const QString& errMsg, const SwAlbum& newAlbum)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Shwup Call Failed: %1", errMsg));
    }
    else
    {
        m_widget->m_albumsCoB->addItem(KIcon("system-users"), newAlbum.title, newAlbum.id);
        m_widget->m_albumsCoB->setCurrentIndex( m_widget->m_albumsCoB->count() - 1 );
    }

    buttonStateChange(true);
}

void SwWindow::slotImageListChanged()
{
    enableButton(User1, !(m_widget->m_imgList->imageUrls().isEmpty()));
}

} // namespace KIPIShwupPlugin
