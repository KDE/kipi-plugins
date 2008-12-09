/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to SmugMug web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

#include "smugmugwindow.h"
#include "smugmugwindow.moc"

// Qt includes.
#include <QFileInfo>
#include <QPushButton>
#include <QCursor>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QProgressDialog>

// KDE includes.
#include <kdeversion.h>
#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmenu.h>
#include <khelpmenu.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kpassworddialog.h>

// LibKExiv2 includes.
#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.
#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// LibKIPI includes.
#include <libkipi/interface.h>
#include <ktoolinvocation.h>
#include "imageslist.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"

// Local includes.
#include "smugmugitem.h"
#include "smugmugtalker.h"
#include "smugmugwidget.h"
#include "smugmugalbum.h"

namespace KIPISmugMugPlugin
{

SmugMugWindow::SmugMugWindow(KIPI::Interface* interface, const QString &tmpFolder, QWidget *parent)
            : KDialog(parent)
{
    setWindowTitle(i18n("Export to SmugMug Web Service"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);

    m_tmpPath.clear();
    m_tmpDir                 = tmpFolder;
    m_interface              = interface;
    m_uploadCount            = 0;
    m_uploadTotal            = 0;
    m_widget                 = new SmugMugWidget(this, interface);

    setButtonGuiItem(User1, KGuiItem(i18n("Start Upload"), KIcon("network-workgroup")));
    setMainWidget(m_widget);
    m_widget->setMinimumSize(700, 500);

    connect(m_widget->m_imgList, SIGNAL( signalImageListChanged(bool) ),
            this, SLOT( slotImageListChanged(bool)) );

    connect(m_widget->m_changeUserBtn, SIGNAL( clicked() ),
            this, SLOT( slotUserChangeRequest()) );

    connect(m_widget->m_reloadAlbumsBtn, SIGNAL( clicked() ),
            this, SLOT( slotReloadAlbumsRequest()) );

    connect(m_widget->m_newAlbumBtn, SIGNAL( clicked() ),
            this, SLOT( slotNewAlbumRequest()) );

    connect(this, SIGNAL( closeClicked() ),
            this, SLOT( slotClose()) );

    connect(this, SIGNAL( user1Clicked() ),
            this, SLOT( slotStartUpload()) );

    // ------------------------------------------------------------------------
    m_about = new KIPIPlugins::KPAboutData(ki18n("SmugMug Export"), 0,
                      KAboutData::License_GPL,
                      ki18n("A Kipi plugin to export image collection to "
                            "SmugMug web service."),
                      ki18n("(c) 2005-2008, Vardhman Jain\n"
                            "(c) 2008, Gilles Caulier\n"
                            "(c) 2008, Luka Renko"));

    m_about->addAuthor(ki18n("Luka Renko"), ki18n("Author and maintainer"),
                       "lure at kubuntu dot org");

    disconnect(this, SIGNAL( helpClicked() ),
               this, SLOT( slotHelp()) );

    KPushButton *helpButton = button(Help);
    KHelpMenu* helpMenu     = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook       = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu(helpMenu->menu());

    // ------------------------------------------------------------------------

    m_loginDlg  = new KPasswordDialog(this, KPasswordDialog::ShowUsernameLine);
    m_loginDlg->setPrompt(i18n("Enter an e-mail and a password of your SmugMug account"));

    // ------------------------------------------------------------------------

    m_albumDlg  = new SmugMugNewAlbum(this);

    connect(m_albumDlg->m_categCoB, SIGNAL( currentIndexChanged(int) ),
            this, SLOT( slotCategorySelectionChanged(int)) );

    // ------------------------------------------------------------------------

    m_talker = new SmugMugTalker(this);

    connect(m_talker, SIGNAL( signalBusy(bool) ),
            this, SLOT( slotBusy(bool) ));

    connect(m_talker, SIGNAL( signalLoginDone(int, const QString&) ),
            this, SLOT( slotLoginDone(int, const QString&) ));

    connect(m_talker, SIGNAL( signalAddPhotoDone(int, const QString&) ),
            this, SLOT( slotAddPhotoDone(int, const QString&) ));

    connect(m_talker, SIGNAL( signalCreateAlbumDone(int, const QString&, int) ),
            this, SLOT( slotCreateAlbumDone(int, const QString&, int) ));

    connect(m_talker, SIGNAL( signalListAlbumsDone(int, const QString&, const QList <SMAlbum>&) ),
            this, SLOT( slotListAlbumsDone(int, const QString&, const QList <SMAlbum>&) ));

    connect(m_talker, SIGNAL( signalListCategoriesDone(int, const QString&, const QList <SMCategory>&) ),
            this, SLOT( slotListCategoriesDone(int, const QString&, const QList <SMCategory>&) ));

    connect(m_talker, SIGNAL( signalListSubCategoriesDone(int, const QString&, const QList <SMCategory>&) ),
            this, SLOT( slotListSubCategoriesDone(int, const QString&, const QList <SMCategory>&) ));

    // ------------------------------------------------------------------------

    m_progressDlg = new QProgressDialog(this);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);

    connect(m_progressDlg, SIGNAL( canceled() ),
            this, SLOT( slotAddPhotoCancel() ));

    // ------------------------------------------------------------------------

    m_authProgressDlg = new QProgressDialog(this);
    m_authProgressDlg->setModal(true);
    m_authProgressDlg->setAutoReset(true);
    m_authProgressDlg->setAutoClose(true);

    connect(m_authProgressDlg, SIGNAL( canceled() ),
            this, SLOT( slotLoginCancel() ));

    m_talker->m_authProgressDlg = m_authProgressDlg;

    // ------------------------------------------------------------------------

    readSettings();

    kDebug(51000) << "Calling Login method";
    m_loggedIn = false;
    buttonStateChange(m_loggedIn);
    if (!m_email.isEmpty())
        m_talker->login(m_email, m_password);
    else
        slotUserChangeRequest();  // pop-up login window
}

SmugMugWindow::~SmugMugWindow()
{
    delete m_progressDlg;
    delete m_authProgressDlg;
    delete m_talker;
    delete m_widget;
    delete m_about;
}

void SmugMugWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("SmugMug Settings");
    m_email = grp.readEntry("Email");
    m_password = grp.readEntry("Password");
    m_currentAlbumID = grp.readEntry("Current Album", -1);

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

    m_widget->m_dimensionSpB->setValue(grp.readEntry("Maximum Width", 1600));
    m_widget->m_imageQualitySpB->setValue(grp.readEntry("Image Quality", 85));
    KConfigGroup dialogGroup = config.group("SmugMug Dialog");
    restoreDialogSize(dialogGroup);
}

void SmugMugWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("SmugMug Settings");
    grp.writeEntry("Email", m_email);
    grp.writeEntry("Password", m_password);
    grp.writeEntry("Current Album", m_currentAlbumID);
    grp.writeEntry("Resize", m_widget->m_resizeChB->isChecked());
    grp.writeEntry("Maximum Width",  m_widget->m_dimensionSpB->value());
    grp.writeEntry("Image Quality",  m_widget->m_imageQualitySpB->value());
    KConfigGroup dialogGroup = config.group("SmugMug Dialog");
    saveDialogSize(dialogGroup);
    config.sync();
}

void SmugMugWindow::slotHelp()
{
    KToolInvocation::invokeHelp("smugmug", "kipi-plugins");
}

void SmugMugWindow::slotClose()
{
    writeSettings();
    done(Close);
}

void SmugMugWindow::slotLoginDone(int errCode, const QString &errMsg)
{
    m_loggedIn = (errCode == 0);
    buttonStateChange(m_loggedIn);
    m_widget->updateLabels(m_talker->getEmail(),
                           m_talker->getDisplayName(),
                           m_talker->getNickName());
    m_widget->m_albumsCoB->clear();

    if (m_loggedIn)
    {
        m_talker->listAlbums(); // get albums to fill combo box
    }
    else
    {
        KMessageBox::error(this, i18n("SmugMug Call Failed: %1\n", errMsg));
    }
}

void SmugMugWindow::slotListAlbumsDone(int errCode, const QString &errMsg,
                                       const QList <SMAlbum>& albumsList)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("SmugMug Call Failed: %1\n", errMsg));
        return;
    }

    m_widget->m_albumsCoB->clear();
    for (int i = 0; i < albumsList.size(); ++i)
    {
        m_widget->m_albumsCoB->addItem(
            albumsList.at(i).title,
            albumsList.at(i).id);
       if (m_currentAlbumID == albumsList.at(i).id)
           m_widget->m_albumsCoB->setCurrentIndex(i);
    }
}

void SmugMugWindow::slotListCategoriesDone(int errCode, const QString &errMsg,
                                           const QList <SMCategory>& categoriesList)
{
    if (errCode != 0) 
    {
        KMessageBox::error(this, i18n("SmugMug Call Failed: %1\n", errMsg));
        return;
    }

    m_albumDlg->m_categCoB->clear();
    for (int i = 0; i < categoriesList.size(); ++i)
    {
        m_albumDlg->m_categCoB->addItem(
            categoriesList.at(i).name,
            categoriesList.at(i).id);
       if (m_currentCategoryID == categoriesList.at(i).id)
           m_albumDlg->m_categCoB->setCurrentIndex(i);
    }
    m_currentCategoryID = m_albumDlg->m_categCoB->itemData(
                          m_albumDlg->m_categCoB->currentIndex()).toInt();
    m_talker->listSubCategories(m_currentCategoryID);
}

void SmugMugWindow::slotListSubCategoriesDone(int errCode, const QString &errMsg,
                                              const QList <SMCategory>& categoriesList)
{
    // always put at least default <none> subcategory
    m_albumDlg->m_subCategCoB->clear();
    m_albumDlg->m_subCategCoB->addItem(i18n("<none>"));

    if (errCode != 0) 
    {
        KMessageBox::error(this, i18n("SmugMug Call Failed: %1\n", errMsg));
        return;
    }

    for (int i = 0; i < categoriesList.size(); ++i)
    {
        m_albumDlg->m_subCategCoB->addItem(
            categoriesList.at(i).name,
            categoriesList.at(i).id);
    }
}

void SmugMugWindow::slotCategorySelectionChanged(int index)
{
    // subcategories are per category -> reload
    m_currentCategoryID = m_albumDlg->m_categCoB->itemData(index).toInt();
    m_talker->listSubCategories(m_currentCategoryID);
}

void SmugMugWindow::buttonStateChange(bool state)
{
    m_widget->m_newAlbumBtn->setEnabled(state);
    m_widget->m_reloadAlbumsBtn->setEnabled(state);
    enableButton(User1, state);
}

void SmugMugWindow::slotBusy(bool val)
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
        buttonStateChange(m_loggedIn);
    }
}

void SmugMugWindow::slotUserChangeRequest()
{
    kDebug(51000) << "Slot Change User Request";

    // fill in current email and password
    m_loginDlg->setUsername(m_email);
    m_loginDlg->setPassword(m_password);

    if (m_loginDlg->exec())
    {
        m_email = m_loginDlg->username();
        m_password = m_loginDlg->password();
        kDebug(51000) << "Calling Login method";
        m_talker->login(m_email, m_password);
    }
}

void SmugMugWindow::slotReloadAlbumsRequest()
{
    kDebug(51000) << "Slot Reload Albums Request";
    m_talker->listAlbums(); // re-get albums
}

void SmugMugWindow::slotNewAlbumRequest()
{
    kDebug(51000) << "Slot New Album Request";

    // get list of Categories from SmugMug to fill in dialog
    m_talker->listCategories();

    if (m_albumDlg->exec() == QDialog::Accepted)
    {
        kDebug(51000) << "Calling New Album method";
        m_currentCategoryID = m_albumDlg->m_categCoB->itemData(
                        m_albumDlg->m_categCoB->currentIndex()).toInt();

        SMAlbum newAlbum;
        m_albumDlg->getAlbumProperties(newAlbum);
        m_talker->createAlbum(newAlbum);
    }
}

void SmugMugWindow::slotLoginCancel()
{
    m_talker->cancel();
    m_authProgressDlg->hide();
}

void SmugMugWindow::slotStartUpload()
{
    kDebug(51000) << "slotStartUpload invoked";

    m_uploadQueue = m_widget->m_imgList->imageUrls();

    if (m_uploadQueue.isEmpty())
        return;

    m_uploadTotal = m_uploadQueue.count();
    m_uploadCount = 0;
    m_progressDlg->reset();
    m_currentAlbumID = m_widget->m_albumsCoB->itemData(
                                m_widget->m_albumsCoB->currentIndex()).toInt();
    kDebug(51000) << "m_currentAlbumID" << m_currentAlbumID;
    uploadNextPhoto();
    kDebug(51000) << "slotStartUpload done";
}

bool SmugMugWindow::prepareImageForUpload(const QString& imgPath, bool isRAW)
{
    QImage image;
    if (isRAW)
    {
        kDebug(51000) << "Get RAW preview " << imgPath;
        KDcrawIface::KDcraw::loadDcrawPreview(image, imgPath);
    }
    else
        image.load(imgPath);

    if (image.isNull())
        return false;

    // get temporary file name
    m_tmpPath = m_tmpDir + QFileInfo(imgPath).baseName().trimmed() + ".jpg";

    // rescale image if requested
    int maxDim = m_widget->m_dimensionSpB->value();
    if (m_widget->m_resizeChB->isChecked()
        && (image.width() > maxDim || image.width() > maxDim))
    {
        kDebug(51000) << "Resizing to " << maxDim;
        image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio);
    }

    kDebug(51000) << "Saving to temp file: " << m_tmpPath;
    image.save(m_tmpPath, "JPEG", m_widget->m_imageQualitySpB->value());

    // copy meta data to temporary image
    KExiv2Iface::KExiv2 exiv2Iface;
    if (exiv2Iface.load(imgPath))
    {
        exiv2Iface.setImageDimensions(image.size());
        exiv2Iface.setImageProgramId("Kipi-plugins", kipiplugins_version);
        exiv2Iface.save(m_tmpPath);
    }
    return true;
}

void SmugMugWindow::uploadNextPhoto()
{
    if (m_uploadQueue.isEmpty())
    {
        m_progressDlg->reset();
        m_progressDlg->hide();
        return;
    }
    QString imgPath = m_uploadQueue.first().path();

    // check if we have to RAW file -> use preview image then
#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    QFileInfo fileInfo(imgPath);
    bool isRAW = rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper());
    bool res;
    if (isRAW || m_widget->m_resizeChB->isChecked()) 
    {
        if (!prepareImageForUpload(imgPath, isRAW))
        {
            slotAddPhotoDone(666, i18n("Cannot open file"));
            return;
        }
        res = m_talker->addPhoto(m_tmpPath, m_currentAlbumID);
    }
    else
    {
        m_tmpPath.clear();
        res = m_talker->addPhoto(imgPath, m_currentAlbumID);
    }
    if (!res)
    {
        slotAddPhotoDone(666, i18n("Cannot open file"));
        return;
    }

    m_progressDlg->setLabelText(i18n("Uploading file %1",
                                     m_uploadQueue.first().path()));

    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void SmugMugWindow::slotAddPhotoDone(int errCode, const QString& errMsg)
{
    // Remove temporary file if it was used
    if (!m_tmpPath.isEmpty()) 
    {
        QFile::remove(m_tmpPath);
        m_tmpPath.clear();
    }

    // Remove photo uploaded from the list
    m_widget->m_imgList->removeItemByUrl(m_uploadQueue.first());
    m_uploadQueue.pop_front();

    if (errCode == 0)
    {
        m_uploadCount++;
    }
    else
    {
        m_uploadTotal--;
        if (KMessageBox::warningContinueCancel(this,
                         i18n("Failed to upload photo into SmugMug: %1\n"
                              "Do you want to continue?", errMsg))
                         != KMessageBox::Continue)
        {
            m_uploadQueue.clear();
            m_progressDlg->reset();
            m_progressDlg->hide();
            return;
        }
    }
    m_progressDlg->setMaximum(m_uploadTotal);
    m_progressDlg->setValue(m_uploadCount);
    uploadNextPhoto();
}

void SmugMugWindow::slotAddPhotoCancel()
{
    m_uploadQueue.clear();
    m_progressDlg->reset();
    m_progressDlg->hide();

    m_talker->cancel();
}

void SmugMugWindow::slotCreateAlbumDone(int errCode, const QString& errMsg,
                                        int newAlbumID)
{
    if (errCode != 0) 
    {
        KMessageBox::error(this, i18n("SmugMug Call Failed: %1\n", errMsg));
        return;
    }

    // reload album list and automaticaly select new album
    m_currentAlbumID = newAlbumID;
    m_talker->listAlbums();
}

void SmugMugWindow::slotImageListChanged(bool state)
{
    enableButton(User1, !state);
}

} // namespace KIPISmugMugPlugin
