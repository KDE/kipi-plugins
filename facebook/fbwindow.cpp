/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#include "fbwindow.h"
#include "fbwindow.moc"

// Qt includes

#include <QFileInfo>
#include <QSpinBox>
#include <QCheckBox>
#include <QCloseEvent>

// KDE includes

#include <KDebug>
#include <KConfig>
#include <KLocale>
#include <KMenu>
#include <KHelpMenu>
#include <KLineEdit>
#include <KComboBox>
#include <KPushButton>
#include <KMessageBox>
#include <KProgressDialog>
#include <KToolInvocation>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// LibKIPI includes

#include <libkipi/interface.h>
#include "imageslist.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"

// Local includes

#include "fbitem.h"
#include "fbtalker.h"
#include "fbwidget.h"
#include "fbalbum.h"

namespace KIPIFacebookPlugin
{

FbWindow::FbWindow(KIPI::Interface* interface, const QString &tmpFolder,
                   bool import, QWidget* /*parent*/)
        : KDialog(0)
{
    m_tmpPath.clear();
    m_tmpDir      = tmpFolder;
    m_interface   = interface;
    m_import      = import;
    m_imagesCount = 0;
    m_imagesTotal = 0;
    m_widget      = new FbWidget(this, interface, import);

    setMainWidget(m_widget);
    setWindowIcon(KIcon("facebook"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);

    if (import)
    {
        setWindowTitle(i18n("Import from Facebook Web Service"));
        setButtonGuiItem(User1,
                         KGuiItem(i18n("Start Download"), "network-workgroup",
                                  i18n("Start download from Facebook web service")));
        m_widget->setMinimumSize(400, 600);
    }
    else
    {
        setWindowTitle(i18n("Export to Facebook Web Service"));
        setButtonGuiItem(User1,
                         KGuiItem(i18n("Start Upload"), "network-workgroup",
                                  i18n("Start upload to Facebook web service")));
        m_widget->setMinimumSize(700, 500);
    }

    // ------------------------------------------------------------------------

    connect(m_widget->m_imgList, SIGNAL( signalImageListChanged()),
            this, SLOT( slotImageListChanged()) );

    connect(m_widget->m_changeUserBtn, SIGNAL( clicked() ),
            this, SLOT( slotUserChangeRequest()) );

    connect(m_widget->m_changePermBtn, SIGNAL( clicked() ),
            this, SLOT( slotPermChangeRequest()) );

    connect(m_widget->m_newAlbumBtn, SIGNAL( clicked() ),
            this, SLOT( slotNewAlbumRequest()) );

    connect(m_widget, SIGNAL( reloadAlbums(long long) ),
            this, SLOT( slotReloadAlbumsRequest(long long)) );

    connect(this, SIGNAL( closeClicked() ),
            this, SLOT( slotClose()) );

    connect(this, SIGNAL( user1Clicked() ),
            this, SLOT( slotStartTransfer()) );

    // ------------------------------------------------------------------------

    m_about = new KIPIPlugins::KPAboutData(ki18n("Facebook Import/Export"), 0,
                      KAboutData::License_GPL,
                      ki18n("A Kipi plugin to import/export image collection "
                            "to/from Facebook web service."),
                      ki18n("(c) 2005-2008, Vardhman Jain\n"
                            "(c) 2008-2009, Gilles Caulier\n"
                            "(c) 2008-2009, Luka Renko"));

    m_about->addAuthor(ki18n("Luka Renko"), ki18n("Author and maintainer"),
                       "lure at kubuntu dot org");

    disconnect(this, SIGNAL( helpClicked() ),
               this, SLOT( slotHelp()) );

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ------------------------------------------------------------------------

    m_albumDlg  = new FbNewAlbum(this);

    // ------------------------------------------------------------------------

    m_talker = new FbTalker(this);

    connect(m_talker, SIGNAL( signalBusy(bool) ),
            this, SLOT( slotBusy(bool) ));

    connect(m_talker, SIGNAL( signalLoginProgress(int, int, const QString&) ),
            this, SLOT( slotLoginProgress(int, int, const QString&) ));

    connect(m_talker, SIGNAL( signalLoginDone(int, const QString&) ),
            this, SLOT( slotLoginDone(int, const QString&) ));

    connect(m_talker, SIGNAL( signalChangePermDone(int, const QString&) ),
            this, SLOT( slotChangePermDone(int, const QString&) ));

    connect(m_talker, SIGNAL( signalAddPhotoDone(int, const QString&) ),
            this, SLOT( slotAddPhotoDone(int, const QString&) ));

    connect(m_talker, SIGNAL( signalGetPhotoDone(int, const QString&, const QByteArray&) ),
            this, SLOT( slotGetPhotoDone(int, const QString&, const QByteArray&) ));

    connect(m_talker, SIGNAL( signalCreateAlbumDone(int, const QString&, long long) ),
            this, SLOT( slotCreateAlbumDone(int, const QString&, long long) ));

    connect(m_talker, SIGNAL( signalListAlbumsDone(int, const QString&, const QList <FbAlbum>&) ),
            this, SLOT( slotListAlbumsDone(int, const QString&, const QList <FbAlbum>&) ));

    connect(m_talker, SIGNAL( signalListPhotosDone(int, const QString&, const QList <FbPhoto>&) ),
            this, SLOT( slotListPhotosDone(int, const QString&, const QList <FbPhoto>&) ));

    connect(m_talker, SIGNAL( signalListFriendsDone(int, const QString&, const QList <FbUser>&) ),
            this, SLOT( slotListFriendsDone(int, const QString&, const QList <FbUser>&) ));

    // ------------------------------------------------------------------------

    readSettings();

    kDebug() << "Calling Login method";
    buttonStateChange(m_talker->loggedIn());
    authenticate();
}

FbWindow::~FbWindow()
{
    delete m_talker;
    delete m_about;
}

void FbWindow::slotHelp()
{
    KToolInvocation::invokeHelp("facebook", "kipi-plugins");
}

void FbWindow::slotClose()
{
    writeSettings();
    m_widget->imagesList()->listView()->clear();
    done(Close);
}

void FbWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void FbWindow::closeEvent(QCloseEvent *e)
{
    if (!e) return;

    writeSettings();
    m_widget->imagesList()->listView()->clear();
    e->accept();
}

void FbWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("Facebook Settings");
    m_sessionKey     = grp.readEntry("Session Key");
    m_sessionSecret  = grp.readEntry("Session Secret");
    m_sessionExpires = grp.readEntry("Session Expires", 0);
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

    m_widget->m_dimensionSpB->setValue(grp.readEntry("Maximum Width", 604));
    m_widget->m_imageQualitySpB->setValue(grp.readEntry("Image Quality", 85));
    if (m_import)
    {
        KConfigGroup dialogGroup = config.group("Facebook Import Dialog");
        restoreDialogSize(dialogGroup);
    }
    else
    {
        KConfigGroup dialogGroup = config.group("Facebook Export Dialog");
        restoreDialogSize(dialogGroup);
    }
}

void FbWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("Facebook Settings");
    grp.writeEntry("Session Key",     m_sessionKey);
    grp.writeEntry("Session Secret",  m_sessionSecret);
    grp.writeEntry("Session Expires", m_sessionExpires);
    grp.writeEntry("Current Album",   m_currentAlbumID);
    grp.writeEntry("Resize",          m_widget->m_resizeChB->isChecked());
    grp.writeEntry("Maximum Width",   m_widget->m_dimensionSpB->value());
    grp.writeEntry("Image Quality",   m_widget->m_imageQualitySpB->value());
    if (m_import)
    {
        KConfigGroup dialogGroup = config.group("Facebook Import Dialog");
        saveDialogSize(dialogGroup);
    }
    else
    {
        KConfigGroup dialogGroup = config.group("Facebook Export Dialog");
        saveDialogSize(dialogGroup);
    }
    config.sync();
}

void FbWindow::authenticate()
{
    m_authProgressDlg = new KProgressDialog(this, i18n("Authentication"));
    m_authProgressDlg->setMinimumDuration(0);
    m_authProgressDlg->setModal(true);
    m_authProgressDlg->setAutoReset(true);
    m_authProgressDlg->setAutoClose(true);

    connect(m_authProgressDlg, SIGNAL( cancelClicked() ),
            this, SLOT( slotLoginCancel() ));

    kDebug() << "Calling Login method";
    m_talker->authenticate(m_sessionKey, m_sessionSecret, m_sessionExpires);
}

void FbWindow::slotLoginProgress(int step, int maxStep, const QString& label)
{
    if (!m_authProgressDlg)
        return;

    if (!label.isEmpty())
        m_authProgressDlg->setLabelText(label);

    if (maxStep > 0)
        m_authProgressDlg->progressBar()->setMaximum(maxStep);

    m_authProgressDlg->progressBar()->setValue(step);
}

void FbWindow::slotLoginDone(int errCode, const QString& errMsg)
{
    m_authProgressDlg->hide();

    buttonStateChange(m_talker->loggedIn());
    FbUser user = m_talker->getUser();
    setProfileAID(user.id);
    m_widget->updateLabels(user.name, user.profileURL, user.uploadPerm);
    m_widget->m_albumsCoB->clear();
    if (!m_import)
        m_widget->m_albumsCoB->addItem(i18n("<auto create>"), 0);

    m_sessionKey     = m_talker->getSessionKey();
    m_sessionSecret  = m_talker->getSessionSecret();
    m_sessionExpires = m_talker->getSessionExpires();

    if (errCode == 0 && m_talker->loggedIn())
    {
        if (m_import)
            m_talker->listFriends();
        else
            m_talker->listAlbums(); // get albums to fill combo box
    }
    else
    {
        KMessageBox::error(this, i18n("Facebook Call Failed: %1\n", errMsg));
    }
}

void FbWindow::slotChangePermDone(int errCode, const QString& errMsg)
{
    if (errCode == 0)
    {
        FbUser user = m_talker->getUser();
        m_widget->updateLabels(user.name, user.profileURL, user.uploadPerm);
    }
    else
    {
        KMessageBox::error(this, i18n("Facebook Call Failed: %1\n", errMsg));
    }
}

void FbWindow::slotListAlbumsDone(int errCode, const QString& errMsg, const QList <FbAlbum>& albumsList)
{

    QString albumDebug = "";
    foreach(FbAlbum album, albumsList)
    {
        albumDebug.append(QString::number(album.id) + ": " + album.title + "\n");
    }
    kDebug() << "Received albums (errCode = " << errCode << ", errMsg = "
                  << errMsg << "): " << albumDebug;

    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Facebook Call Failed: %1\n", errMsg));
        return;
    }

    m_widget->m_albumsCoB->clear();
    if (m_import)
    {
        m_widget->m_albumsCoB->addItem(
            i18nc("name of special Facebook album for profile pictures",
                  "Profile Pictures"),
            m_profileAID);
    }
    else
    {
        m_widget->m_albumsCoB->addItem(i18n("<auto create>"), 0);
    }

    for (int i = 0; i < albumsList.size(); ++i)
    {
        QString albumIcon;
        switch (albumsList.at(i).privacy)
        {
            case FB_FRIENDS:
                albumIcon = "user-identity";
                break;
            case FB_FRIENDS_OF_FRIENDS:
                albumIcon = "system-users";
                break;
            case FB_NETWORKS:
                albumIcon = "network-workgroup";
                break;
            case FB_EVERYONE:
                albumIcon = "applications-internet";
                break;
        }
        m_widget->m_albumsCoB->addItem(
            KIcon(albumIcon),
            albumsList.at(i).title,
            albumsList.at(i).id);
       if (m_currentAlbumID == albumsList.at(i).id)
           m_widget->m_albumsCoB->setCurrentIndex(i+1);
    }
}

void FbWindow::slotListPhotosDone(int errCode, const QString& errMsg, const QList <FbPhoto>& photosList)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Facebook Call Failed: %1\n", errMsg));
        return;
    }

    m_transferQueue.clear();
    for (int i = 0; i < photosList.size(); ++i)
    {
        m_transferQueue.push_back(photosList.at(i).originalURL);
    }

    if (m_transferQueue.isEmpty())
        return;

    m_imagesTotal = m_transferQueue.count();
    m_imagesCount = 0;

    m_progressDlg = new KProgressDialog(this, i18n("Transfer Progress"));
    m_progressDlg->setMinimumDuration(0);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);
    m_progressDlg->progressBar()->setFormat(i18n("%v / %m"));

    connect(m_progressDlg, SIGNAL( cancelClicked() ),
            this, SLOT( slotTransferCancel() ));

    // start download with first photo in queue
    downloadNextPhoto();
}

void FbWindow::slotListFriendsDone(int errCode, const QString& errMsg, const QList <FbUser>& friendsList)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Facebook Call Failed: %1\n", errMsg));
        return;
    }

    m_widget->m_friendsCoB->clear();
    for (int i = 0; i < friendsList.size(); ++i)
    {
        m_widget->m_friendsCoB->addItem(
            friendsList.at(i).name,
            friendsList.at(i).id);
    }
    m_widget->m_friendsCoB->setCurrentIndex(-1); // no friend selected
    m_talker->listAlbums(); // now fill up albums list with user's albums
}

void FbWindow::buttonStateChange(bool state)
{
    m_widget->m_changePermBtn->setEnabled(state);
    m_widget->m_newAlbumBtn->setEnabled(state);
    m_widget->m_reloadAlbumsBtn->setEnabled(state);
    enableButton(User1, state);
}

void FbWindow::slotBusy(bool val)
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
        buttonStateChange(m_talker->loggedIn());
    }
}

void FbWindow::slotUserChangeRequest()
{
    kDebug() << "Slot Change User Request";

    if (m_talker->loggedIn())
    {
        m_talker->logout();
        m_sessionKey.clear();
        m_sessionSecret.clear();
        m_sessionExpires = 0;
    }

    authenticate();
}

void FbWindow::slotPermChangeRequest()
{
    kDebug() << "Slot Change Permission Request";

    kDebug() << "Calling Login method";
    m_talker->changePerm();
}

void FbWindow::slotReloadAlbumsRequest(long long userID)
{
    kDebug() << "Reload Albums Request for UID:" << userID;
    if (userID == 0)
    {
        FbUser user = m_talker->getUser();
        setProfileAID(user.id);
        m_talker->listAlbums(); // re-get albums from current user
    }
    else
    {
        setProfileAID(userID);
        m_talker->listAlbums(userID); // re-get albums for friend
    }
}

void FbWindow::slotNewAlbumRequest()
{
    kDebug() << "Slot New Album Request";

    if (m_albumDlg->exec() == QDialog::Accepted)
    {
        kDebug() << "Calling New Album method";
        FbAlbum newAlbum;
        m_albumDlg->getAlbumProperties(newAlbum);
        m_talker->createAlbum(newAlbum);
    }
}

void FbWindow::slotLoginCancel()
{
    m_talker->cancel();
    m_authProgressDlg->hide();
}

void FbWindow::slotStartTransfer()
{
    kDebug() << "slotStartTransfer invoked";

    if (m_import)
    {
        m_talker->listPhotos(m_widget->getFriendID(), m_widget->getAlbumID());
    }
    else
    {
        m_transferQueue = m_widget->m_imgList->imageUrls();

        if (m_transferQueue.isEmpty())
            return;

        m_currentAlbumID = m_widget->m_albumsCoB->itemData(
                                   m_widget->m_albumsCoB->currentIndex()).toLongLong();
        kDebug() << "upload request got album id from widget: " << m_currentAlbumID;
        m_imagesTotal = m_transferQueue.count();
        m_imagesCount = 0;

        m_progressDlg = new KProgressDialog(this, i18n("Transfer Progress"));
        m_progressDlg->setMinimumDuration(0);
        m_progressDlg->setModal(true);
        m_progressDlg->setAutoReset(true);
        m_progressDlg->setAutoClose(true);
        m_progressDlg->progressBar()->setFormat(i18n("%v / %m"));

        connect(m_progressDlg, SIGNAL( cancelClicked() ),
                this, SLOT( slotTransferCancel() ));

        uploadNextPhoto();
    }
}

void FbWindow::setProfileAID(long long userID)
{
    // store AID of Profile Photos album
    // http://wiki.developers.facebook.com/index.php/Profile_archive_album
    m_profileAID    = (userID << 32) + (-3 & 0xFFFFFFFF);
}

QString FbWindow::getImageCaption(const KExiv2Iface::KExiv2& ev)
{
    QString caption = ev.getCommentsDecoded();
    if (!caption.isEmpty())
        return caption;

    if (ev.hasExif())
    {
        caption = ev.getExifComment();
        if (!caption.isEmpty())
            return caption;
    }

    if (ev.hasXmp())
    {
        caption = ev.getXmpTagStringLangAlt("Xmp.dc.description", QString(), false);
        if (!caption.isEmpty())
            return caption;

        caption = ev.getXmpTagStringLangAlt("Xmp.exif.UserComment", QString(), false);
        if (!caption.isEmpty())
            return caption;

        caption = ev.getXmpTagStringLangAlt("Xmp.tiff.ImageDescription", QString(), false);
        if (!caption.isEmpty())
            return caption;
}

    if (ev.hasIptc())
    {
        caption = ev.getIptcTagString("Iptc.Application2.Caption", false);
        if (!caption.isEmpty() && !caption.trimmed().isEmpty())
            return caption;
    }

    return caption;
}

bool FbWindow::prepareImageForUpload(const QString& imgPath, bool isRAW, QString& caption)
{
    QImage image;
    if (isRAW)
    {
        kDebug() << "Get RAW preview " << imgPath;
        KDcrawIface::KDcraw::loadDcrawPreview(image, imgPath);
    }
    else
    {
        image.load(imgPath);
    }

    if (image.isNull())
        return false;

    // get temporary file name
    m_tmpPath = m_tmpDir + QFileInfo(imgPath).baseName().trimmed() + ".jpg";

    // rescale image if requested
    int maxDim = m_widget->m_dimensionSpB->value();
    if (m_widget->m_resizeChB->isChecked()
        && (image.width() > maxDim || image.height() > maxDim))
    {
        kDebug() << "Resizing to " << maxDim;
        image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
    }

    kDebug() << "Saving to temp file: " << m_tmpPath;
    image.save(m_tmpPath, "JPEG", m_widget->m_imageQualitySpB->value());

    // copy meta data to temporary image
    KExiv2Iface::KExiv2 exiv2Iface;
    if (exiv2Iface.load(imgPath))
    {
        caption = getImageCaption(exiv2Iface);
        exiv2Iface.setImageDimensions(image.size());
        exiv2Iface.setImageProgramId("Kipi-plugins", kipiplugins_version);
        exiv2Iface.save(m_tmpPath);
    }
    else
    {
        caption.clear();
    }

    return true;
}

void FbWindow::uploadNextPhoto()
{
    if (m_transferQueue.isEmpty())
    {
        m_progressDlg->hide();
        return;
    }

    m_progressDlg->progressBar()->setMaximum(m_imagesTotal);
    m_progressDlg->progressBar()->setValue(m_imagesCount);

    QString imgPath = m_transferQueue.first().path();

    // check if we have to RAW file -> use preview image then
#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    QFileInfo fileInfo(imgPath);
    QString caption;
    bool isRAW = rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper());
    bool res;
    if (isRAW || m_widget->m_resizeChB->isChecked())
    {
        if (!prepareImageForUpload(imgPath, isRAW, caption))
        {
            slotAddPhotoDone(666, i18n("Cannot open file"));
            return;
        }
        res = m_talker->addPhoto(m_tmpPath, m_currentAlbumID, caption);
    }
    else
    {
        KExiv2Iface::KExiv2 exiv2Iface;

        if (exiv2Iface.load(imgPath))
            caption = getImageCaption(exiv2Iface);
        else
            caption.clear();

        m_tmpPath.clear();
        res = m_talker->addPhoto(imgPath, m_currentAlbumID, caption);
    }
    if (!res)
    {
        slotAddPhotoDone(666, i18n("Cannot open file"));
        return;
    }

    m_progressDlg->setLabelText(i18n("Uploading file %1",
                                     m_transferQueue.first().path()));
}

void FbWindow::slotAddPhotoDone(int errCode, const QString& errMsg)
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
                         i18n("Failed to upload photo into Facebook: %1\n"
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

void FbWindow::downloadNextPhoto()
{
    if (m_transferQueue.isEmpty())
    {
        m_progressDlg->hide();
        return;
    }

    m_progressDlg->progressBar()->setMaximum(m_imagesTotal);
    m_progressDlg->progressBar()->setValue(m_imagesCount);

    QString imgPath = m_transferQueue.first().url();

    m_talker->getPhoto(imgPath);

    m_progressDlg->setLabelText(i18n("Downloading file %1", imgPath));
}

void FbWindow::slotGetPhotoDone(int errCode, const QString& errMsg, const QByteArray& photoData)
{
    QString imgPath = m_widget->getDestinationPath() + '/'
                      + QFileInfo(m_transferQueue.first().path()).fileName();
    m_transferQueue.pop_front();

    if (errCode == 0)
    {
        QString errText;
        QFile imgFile(imgPath);
        if (!imgFile.open(QIODevice::WriteOnly))
        {
            errText = imgFile.errorString();
        }
        else if (imgFile.write(photoData) != photoData.size())
        {
            errText = imgFile.errorString();
        }
        else
        {
            imgFile.close();
        }

        if (errText.isEmpty())
        {
            m_imagesCount++;
        }
        else
        {
            m_imagesTotal--;
            if (KMessageBox::warningContinueCancel(this,
                             i18n("Failed to save photo: %1\n"
                                  "Do you want to continue?", errText))
                             != KMessageBox::Continue)
            {
                m_transferQueue.clear();
                m_progressDlg->hide();
                return;
            }
        }
    }
    else
    {
        m_imagesTotal--;
        if (KMessageBox::warningContinueCancel(this,
                         i18n("Failed to download photo: %1\n"
                              "Do you want to continue?", errMsg))
                         != KMessageBox::Continue)
        {
            m_transferQueue.clear();
            m_progressDlg->hide();
            return;
        }
    }

    downloadNextPhoto();
}

void FbWindow::slotTransferCancel()
{
    m_transferQueue.clear();
    m_progressDlg->hide();

    m_talker->cancel();
}

void FbWindow::slotCreateAlbumDone(int errCode, const QString& errMsg, long long newAlbumID)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Facebook Call Failed: %1", errMsg));
        return;
    }

    // reload album list and automatically select new album
    m_currentAlbumID = newAlbumID;
    m_talker->listAlbums();
}

void FbWindow::slotImageListChanged()
{
    enableButton(User1, !(m_widget->m_imgList->imageUrls().isEmpty()));
}

} // namespace KIPIFacebookPlugin
