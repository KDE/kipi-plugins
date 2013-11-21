/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#include "gdwindow.moc"

// Qt includes

#include <QPushButton>
#include <QProgressDialog>
#include <QPixmap>
#include <QCheckBox>
#include <QStringList>
#include <QSpinBox>
#include <QPointer>

// KDE includes

#include <kcombobox.h>
#include <klineedit.h>
#include <kmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <ktabwidget.h>
#include <krun.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kdeversion.h>
#include <kwallet.h>
#include <kpushbutton.h>
#include <kurl.h>
#include <ktoolinvocation.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpimageslist.h"
#include "kprogressdialog.h"
#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpprogresswidget.h"
#include "gdtalker.h"
#include "gditem.h"
#include "gdalbum.h"
#include "gdwidget.h"

namespace KIPIGoogleDrivePlugin
{

GDWindow::GDWindow(const QString& tmpFolder,QWidget* const /*parent*/)
    : KPToolDialog(0)
{
    m_tmp         = tmpFolder;
    m_imagesCount = 0;
    m_imagesTotal = 0;

    m_widget      = new GoogleDriveWidget(this);
    setMainWidget(m_widget);
    setWindowIcon(KIcon("kipi-googledrive"));
    setButtons(Help | User1 | Close);
    setDefaultButton(Close);
    setModal(false);
    setWindowTitle(i18n("Export to Google Drive"));
    setButtonGuiItem(User1,KGuiItem(i18n("Start-Upload"),"network-workgroup",i18n("Start upload to Google Drive")));
    m_widget->setMinimumSize(700,500);

    connect(m_widget->m_imgList, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(m_widget->m_changeUserBtn,SIGNAL(clicked()),
            this,SLOT(slotUserChangeRequest()));

    connect(m_widget->m_newAlbumBtn,SIGNAL(clicked()),
            this,SLOT(slotNewAlbumRequest()));

    connect(m_widget->m_reloadAlbumsBtn,SIGNAL(clicked()),
            this,SLOT(slotReloadAlbumsRequest()));

    connect(this,SIGNAL(user1Clicked()),
            this,SLOT(slotStartTransfer()));

    KPAboutData* const about = new KPAboutData(ki18n("Google Drive Export"),0,
                                               KAboutData::License_GPL,
                                               ki18n("A Kipi-plugin to export images "
                                                     "to Google Drive"),
                                               ki18n("(c) 2013, Saurabh Patel"));

    about->addAuthor(ki18n("Saurabh Patel"),ki18n("Author and maintainer"),
                     "saurabhpatel7717 at gmail dot com");
    about->setHandbookEntry("googledrive");
    setAboutData(about);

    //-------------------------------------------------------------------------

    m_albumDlg = new GDNewAlbum(this);

    //-------------------------------------------------------------------------

    m_talker = new GDTalker(this);

    connect(m_talker,SIGNAL(signalBusy(bool)),
            this,SLOT(slotBusy(bool)));

    connect(m_talker,SIGNAL(signalTextBoxEmpty()),
            this,SLOT(slotTextBoxEmpty()));

    connect(m_talker,SIGNAL(signalAccessTokenFailed(int,QString)),
            this,SLOT(slotAccessTokenFailed(int,QString)));

    connect(m_talker,SIGNAL(signalAccessTokenObtained()),
            this,SLOT(slotAccessTokenObtained()));

    connect(m_talker,SIGNAL(signalRefreshTokenObtained(QString)),
            this,SLOT(slotRefreshTokenObtained(QString)));

    connect(m_talker,SIGNAL(signalSetUserName(QString)),
            this,SLOT(slotSetUserName(QString)));

    connect(m_talker,SIGNAL(signalListAlbumsFailed(QString)),
            this,SLOT(slotListAlbumsFailed(QString)));

    connect(m_talker,SIGNAL(signalListAlbumsDone(QList<QPair<QString,QString> >)),
            this,SLOT(slotListAlbumsDone(QList<QPair<QString,QString> >)));

    connect(m_talker,SIGNAL(signalCreateFolderFailed(QString)),
            this,SLOT(slotCreateFolderFailed(QString)));

    connect(m_talker,SIGNAL(signalCreateFolderSucceeded()),
            this,SLOT(slotCreateFolderSucceeded()));

    connect(m_talker,SIGNAL(signalAddPhotoFailed(QString)),
            this,SLOT(slotAddPhotoFailed(QString)));

    connect(m_talker,SIGNAL(signalAddPhotoSucceeded()),
            this,SLOT(slotAddPhotoSucceeded()));

    readSettings();
    buttonStateChange(false);

    if(refresh_token.isEmpty())
    {
        m_talker->doOAuth();
    }
    else
    {
        m_talker->getAccessTokenFromRefreshToken(refresh_token);
    }
}

GDWindow::~GDWindow()
{
    delete m_widget;
    delete m_albumDlg;
    delete m_talker;
}

void GDWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    m_widget->progressBar()->hide();

    show();
}

void GDWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("Google Drive Settings");

    m_currentAlbumId = grp.readEntry("Current Album",QString());
    refresh_token = grp.readEntry("refresh_token");

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

    KConfigGroup dialogGroup = config.group("Google Drive Export Dialog");
    restoreDialogSize(dialogGroup);
}

void GDWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("Google Drive Settings");
    grp.writeEntry("refresh_token",refresh_token);
    grp.writeEntry("Current Album",m_currentAlbumId);
    grp.writeEntry("Resize",          m_widget->m_resizeChB->isChecked());
    grp.writeEntry("Maximum Width",   m_widget->m_dimensionSpB->value());
    grp.writeEntry("Image Quality",   m_widget->m_imageQualitySpB->value());

    KConfigGroup dialogGroup = config.group("Google Drive Export Dialog");
    saveDialogSize(dialogGroup);

    config.sync();
}

void GDWindow::slotSetUserName(const QString& msg)
{
    m_widget->updateLabels(msg,"");
}

void GDWindow::slotListAlbumsDone(const QList<QPair<QString,QString> >& list)
{
    m_widget->m_albumsCoB->clear();
    kDebug() << "slotListAlbumsDone1:" << list.size();

    for(int i=0;i<list.size();i++)
    {
        m_widget->m_albumsCoB->addItem(KIcon("system-users"),list.value(i).second,
                                       list.value(i).first);

        if (m_currentAlbumId == list.value(i).first)
        {
            m_widget->m_albumsCoB->setCurrentIndex(i);
        }
    }

    buttonStateChange(true);
    m_talker->getUserName();
}

void GDWindow::slotBusy(bool val)
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

void GDWindow::slotTextBoxEmpty()
{
    kDebug() << "in slotTextBoxEmpty";
    KMessageBox::error(this, i18n("Text Box is Empty, Please Enter code from browser to textbox. To complete authentication press"
                                  " Change Account or start-upload button to authenticate again"));
}

void GDWindow::slotStartTransfer()
{
    m_widget->m_imgList->clearProcessedStatus();

    if(m_widget->m_imgList->imageUrls().isEmpty())
    {
        if (KMessageBox::warningContinueCancel(this, i18n("No Image Selected. Cannot upload.Continue by selecting image "))
            == KMessageBox::Continue)
        {
             return;
        }

        return;
    }

    if(!(m_talker->authenticated()))
    {
        if (KMessageBox::warningContinueCancel(this, i18n("Authentication failed. Press Continue to authenticate"))
            == KMessageBox::Continue)
        {
            m_talker->doOAuth();
            return;
        }
        else
        {
            return;
        }
    }

    typedef QPair<KUrl, GDPhoto> Pair;

    for(int i=0 ;i < (m_widget->m_imgList->imageUrls().size()) ; i++)
    {
        KPImageInfo info(m_widget->m_imgList->imageUrls().value(i));
        GDPhoto temp;
        kDebug() << "in start transfer info " <<info.title() << info.description();
        temp.title          = info.title();
        temp.description    = info.description().section("\n",0,0);

        m_transferQueue.append(Pair(m_widget->m_imgList->imageUrls().value(i),temp));
    }

    m_currentAlbumId = m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString();
    m_imagesTotal    = m_transferQueue.count();
    m_imagesCount    = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(0);
    m_widget->progressBar()->show();
    m_widget->progressBar()->progressScheduled(i18n("GoogleDrive export"), true, true);
    m_widget->progressBar()->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));

    uploadNextPhoto();
}

void GDWindow::uploadNextPhoto()
{
    kDebug() << "in upload nextphoto " << m_transferQueue.count();

    if(m_transferQueue.isEmpty())
    {
        //m_widget->progressBar()->hide();
        m_widget->progressBar()->progressCompleted();
        return;
    }

    typedef QPair<KUrl,GDPhoto> Pair;
    Pair pathComments = m_transferQueue.first();
    GDPhoto info      = pathComments.second;

    bool res = m_talker->addPhoto(pathComments.first.toLocalFile(),info,m_currentAlbumId,
                                  m_widget->m_resizeChB->isChecked(),
                                  m_widget->m_dimensionSpB->value(),
                                  m_widget->m_imageQualitySpB->value());

    if (!res)
    {
        slotAddPhotoFailed("");
        return;
    }
}

void GDWindow::slotAddPhotoFailed(const QString& msg)
{
    if (KMessageBox::warningContinueCancel(this, i18n("Failed to upload photo to Google Drive. %1\nDo you want to continue?",msg))
        != KMessageBox::Continue)
    {
        m_transferQueue.clear();
        m_widget->progressBar()->hide();
    }
    else
    {
        m_transferQueue.pop_front();
        m_imagesTotal--;
        m_widget->progressBar()->setMaximum(m_imagesTotal);
        m_widget->progressBar()->setValue(m_imagesCount);
        uploadNextPhoto();
    }
}

void GDWindow::slotAddPhotoSucceeded()
{
    // Remove photo uploaded from the list
    m_widget->m_imgList->removeItemByUrl(m_transferQueue.first().first);
    m_transferQueue.pop_front();
    m_imagesCount++;
    kDebug() << "In slotAddPhotoSucceeded" << m_imagesCount;
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(m_imagesCount);
    uploadNextPhoto();
}

void GDWindow::slotImageListChanged()
{
    enableButton(User1, !(m_widget->m_imgList->imageUrls().isEmpty()));
}

void GDWindow::slotNewAlbumRequest()
{
    if (m_albumDlg->exec() == QDialog::Accepted)
    {
        GDFolder newFolder;
        m_albumDlg->getAlbumTitle(newFolder);
        m_currentAlbumId = m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString();
        m_talker->createFolder(newFolder.title,m_currentAlbumId);
    }
}

void GDWindow::slotReloadAlbumsRequest()
{
    m_talker->listFolders();
}

void GDWindow::slotAccessTokenFailed(int errCode,const QString& errMsg)
{
    KMessageBox::error(this, i18n("There seems to be %1 error: %2",errCode,errMsg));
    return;
}

void GDWindow::slotAccessTokenObtained()
{
    m_talker->listFolders();
}

void GDWindow::slotRefreshTokenObtained(const QString& msg)
{
    refresh_token = msg;
    m_talker->listFolders();
}

void GDWindow::slotListAlbumsFailed(const QString& msg)
{
    KMessageBox::error(this, i18n("GoogleDrive Call Failed: %1\n", msg));
    return;
}

void GDWindow::slotCreateFolderFailed(const QString& msg)
{
    KMessageBox::error(this, i18n("GoogleDrive Call Failed: %1\n", msg));
}

void GDWindow::slotCreateFolderSucceeded()
{
    m_talker->listFolders();
}

void GDWindow::slotTransferCancel()
{
    m_transferQueue.clear();
    m_progressDlg->hide();
    m_talker->cancel();
}

void GDWindow::slotUserChangeRequest()
{
    KUrl url("https://accounts.google.com/logout");
    KToolInvocation::invokeBrowser(url.url());

    if (KMessageBox::warningContinueCancel(this, i18n("After you have been logged out in the browser,Press 'Continue' to authenticate "
                                                      " for other account"))
        == KMessageBox::Continue)
    {
        refresh_token = "";
        m_talker->doOAuth();
    }
}

void GDWindow::buttonStateChange(bool state)
{
    m_widget->m_newAlbumBtn->setEnabled(state);
    m_widget->m_reloadAlbumsBtn->setEnabled(state);
    enableButton(User1, state);
}

void GDWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
        return;

    writeSettings();
    m_widget->imagesList()->listView()->clear();
    e->accept();
}

} // namespace KIPIGoogleDrivePlugin
