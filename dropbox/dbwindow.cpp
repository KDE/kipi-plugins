/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to import/export images to Dropbox web service
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

#include "dbwindow.moc"

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
#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpprogresswidget.h"
#include "dbtalker.h"
#include "dbitem.h"
#include "dbalbum.h"
#include "dbwidget.h"

namespace KIPIDropboxPlugin
{

DBWindow::DBWindow(const QString& tmpFolder, QWidget* const /*parent*/)
    : KPToolDialog(0)
{
    m_tmp         = tmpFolder;
    m_imagesCount = 0;
    m_imagesTotal = 0;

    m_widget      = new DropboxWidget(this);
    setMainWidget(m_widget);
    setWindowIcon(KIcon("kipi-dropbox"));
    setButtons(Help | User1 | Close);
    setDefaultButton(Close);
    setModal(false);
    setWindowTitle(i18n("Export to Dropbox"));
    setButtonGuiItem(User1,KGuiItem(i18n("Start Upload"),"network-workgroup",i18n("Start upload to Dropbox")));
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

    KPAboutData* const about = new KPAboutData(ki18n("Dropbox Export"),0,
                                               KAboutData::License_GPL,
                                               ki18n("A Kipi-plugin to export images "
                                                     "to Dropbox"),
                                               ki18n("(c) 2013, Saurabh Patel\n"
                                                     "(c) 2015, Shourya Singh Gupta"));

    about->addAuthor(ki18n("Saurabh Patel"),ki18n("Author and maintainer"),
                     "saurabhpatel7717 at gmail dot com");
        about->addAuthor(ki18n("Shourya Singh Gupta"),ki18n("Developer"),
                     "shouryasgupta at gmail dot com");
    about->setHandbookEntry("dropbox");
    setAboutData(about);

    //-------------------------------------------------------------------------

    m_albumDlg = new DBNewAlbum(this);

    //-------------------------------------------------------------------------

    m_talker   = new DBTalker(this);

    connect(m_talker,SIGNAL(signalBusy(bool)),
            this,SLOT(slotBusy(bool)));

    connect(m_talker,SIGNAL(signalTextBoxEmpty()),
            this,SLOT(slotTextBoxEmpty()));

    connect(m_talker,SIGNAL(signalAccessTokenFailed()),
            this,SLOT(slotAccessTokenFailed()));

    connect(m_talker,SIGNAL(signalAccessTokenObtained(QString,QString,QString)),
            this,SLOT(slotAccessTokenObtained(QString,QString,QString)));

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

    if(m_accToken.isEmpty())
    {
        m_talker->obtain_req_token();
    }
    else
    {
        m_talker->continueWithAccessToken(m_accToken,m_accTokenSecret,m_accoauthToken);
    }
}

DBWindow::~DBWindow()
{
    delete m_widget;
    delete m_albumDlg;
    delete m_talker;
}

void DBWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    m_widget->progressBar()->hide();

    show();
}

void DBWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp   = config.group("Dropbox Settings");

    m_currentAlbumName = grp.readEntry("Current Album",QString());
    m_accToken         = grp.readEntry("Access Token");
    m_accTokenSecret   = grp.readEntry("Access Secret");
    m_accoauthToken    = grp.readEntry("Access Oauth Token");

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

    KConfigGroup dialogGroup = config.group("Dropbox Export Dialog");
    restoreDialogSize(dialogGroup);
}

void DBWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("Dropbox Settings");

    grp.writeEntry("Current Album",      m_currentAlbumName);
    grp.writeEntry("Resize",             m_widget->m_resizeChB->isChecked());
    grp.writeEntry("Maximum Width",      m_widget->m_dimensionSpB->value());
    grp.writeEntry("Image Quality",      m_widget->m_imageQualitySpB->value());
    grp.writeEntry("Access Token",       m_accToken);
    grp.writeEntry("Access Secret",      m_accTokenSecret);
    grp.writeEntry("Access Oauth Token", m_accoauthToken);

    KConfigGroup dialogGroup = config.group("Dropbox Export Dialog");
    saveDialogSize(dialogGroup);

    config.sync();
}

void DBWindow::slotSetUserName(const QString& msg)
{
    m_widget->updateLabels(msg,"");
}

void DBWindow::slotListAlbumsDone(const QList<QPair<QString,QString> >& list)
{
    m_widget->m_albumsCoB->clear();
    kDebug() << "slotListAlbumsDone1:" << list.size();
    
    for(int i=0;i<list.size();i++)
    {
        m_widget->m_albumsCoB->addItem(KIcon("system-users"),list.value(i).second,
                                       list.value(i).first);

        if (m_currentAlbumName == list.value(i).first)
        {
            m_widget->m_albumsCoB->setCurrentIndex(i);
        }
    }

    buttonStateChange(true);
    m_talker->getUserName();
}

void DBWindow::slotBusy(bool val)
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

void DBWindow::slotTextBoxEmpty()
{
    KMessageBox::error(this, i18n("The textbox is empty, please enter the code from the browser in the textbox. "
                                  "To complete the authentication click \"Change Account\", "
                                  "or \"Start Upload\" to authenticate again."));

}

void DBWindow::slotStartTransfer()
{
    m_widget->m_imgList->clearProcessedStatus();

    if(m_widget->m_imgList->imageUrls().isEmpty())
    {
        if (KMessageBox::warningContinueCancel(this, i18n("No image selected. Please select which images should be uploaded."))
            == KMessageBox::Continue)
        {
             return;
        }
        return;
    }

    if(!(m_talker->authenticated()))
    {
        if (KMessageBox::warningContinueCancel(this, i18n("Authentication failed. Click \"Continue\" to authenticate."))
            == KMessageBox::Continue)
        {
            m_talker->obtain_req_token();
            return;
        }
        else
        {
            return;
        }
    }

    m_transferQueue = m_widget->m_imgList->imageUrls();

    if(m_transferQueue.isEmpty())
    {
        return;
    }

    m_currentAlbumName = m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString();

    m_imagesTotal = m_transferQueue.count();
    m_imagesCount = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(0);
    m_widget->progressBar()->show();
    m_widget->progressBar()->progressScheduled(i18n("Dropbox export"), true, true);
    m_widget->progressBar()->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));

    uploadNextPhoto();
}

void DBWindow::uploadNextPhoto()
{
    kDebug() << "in upload nextphoto " << m_transferQueue.count();

    if(m_transferQueue.isEmpty())
    {
        kDebug() << "empty";
        m_widget->progressBar()->progressCompleted();
        return;
    }

    QString imgPath = m_transferQueue.first().path();
    QString temp = m_currentAlbumName + QString("/");

    bool res = m_talker->addPhoto(imgPath,temp,m_widget->m_resizeChB->isChecked(),m_widget->m_dimensionSpB->value(),
                                  m_widget->m_imageQualitySpB->value());

    if (!res)
    {
        slotAddPhotoFailed("");
        return;
    }
}

void DBWindow::slotAddPhotoFailed(const QString& msg)
{
    if (KMessageBox::warningContinueCancel(this, i18n("Failed to upload photo to Dropbox.\n%1\nDo you want to continue?",msg))
        != KMessageBox::Continue)
    {
        m_transferQueue.clear();
        m_widget->progressBar()->hide();
        kDebug() << "In slotAddPhotoFailed 1";
        // refresh the thumbnails
        //slotTagSelected();
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

void DBWindow::slotAddPhotoSucceeded()
{
    // Remove photo uploaded from the list
    m_widget->m_imgList->removeItemByUrl(m_transferQueue.first());
    m_transferQueue.pop_front();
    m_imagesCount++;
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(m_imagesCount);
    uploadNextPhoto();
}

void DBWindow::slotImageListChanged()
{
    enableButton(User1, !(m_widget->m_imgList->imageUrls().isEmpty()));
}

void DBWindow::slotNewAlbumRequest()
{
    if (m_albumDlg->exec() == QDialog::Accepted)
    {
        DBFolder newFolder;
        m_albumDlg->getFolderTitle(newFolder);
        kDebug() << "in slotNewAlbumRequest() " << newFolder.title;
        m_currentAlbumName = m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString();
        QString temp = m_currentAlbumName + newFolder.title;
        m_talker->createFolder(temp);
    }
}

void DBWindow::slotReloadAlbumsRequest()
{
    m_talker->listFolders("/");
}

void DBWindow::slotAccessTokenFailed()
{
    if (KMessageBox::warningContinueCancel(this, i18n("Authentication failed. Click \"Continue\" to authenticate."))
        == KMessageBox::Continue)
    {
        m_talker->obtain_req_token();
        return;
    }
    else
    {
        return;
    }
}

void DBWindow::slotAccessTokenObtained(const QString& msg1,const QString& msg2,const QString& msg3)
{
    kDebug() << "acc : 111";
    m_accToken       = msg1;
    m_accTokenSecret = msg2;
    m_accoauthToken  = msg3;

    m_talker->listFolders("/");
}

void DBWindow::slotListAlbumsFailed(const QString& msg)
{
    KMessageBox::error(this, i18n("Dropbox call failed:\n%1", msg));
    return;
}

void DBWindow::slotCreateFolderFailed(const QString& msg)
{
    KMessageBox::error(this, i18n("Dropbox call failed:\n%1", msg));
    //return;
}

void DBWindow::slotCreateFolderSucceeded()
{
    m_talker->listFolders("/");
}

void DBWindow::slotTransferCancel()
{
    m_transferQueue.clear();
    m_widget->progressBar()->hide();
    m_talker->cancel();
}

void DBWindow::slotUserChangeRequest()
{
    m_accToken = "";
    m_accTokenSecret = "";
    m_accoauthToken = "";
    m_talker->obtain_req_token();
}

void DBWindow::buttonStateChange(bool state)
{
    m_widget->m_newAlbumBtn->setEnabled(state);
    m_widget->m_reloadAlbumsBtn->setEnabled(state);
    enableButton(User1, state);
}

void DBWindow::closeEvent(QCloseEvent* e)
{
    if (!e) return;

    writeSettings();
    m_widget->imagesList()->listView()->clear();
    e->accept();
}

} // namespace KIPIDropboxPlugin
