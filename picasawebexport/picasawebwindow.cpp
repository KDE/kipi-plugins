/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009 by Luka Renko <lure at kubuntu dot org>
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

#include "picasawebwindow.h"
#include "picasawebwindow.moc"

// Qt includes

#include <QProgressDialog>
#include <QCloseEvent>
#include <QDateTime>
#include <QLinkedList>
#include <QStringList>
#include <QPointer>

// KDE includes

#include <kapplication.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <khelpmenu.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <krun.h>
#include <ksimpleconfig.h>
#include <kwallet.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <ktoolinvocation.h>

// Local includes

#include "imagedialog.h"
#include "kpaboutdata.h"
#include "newalbumdialog.h"
#include "picasawebitem.h"
#include "picasaweblogin.h"
#include "picasawebtalker.h"
#include "picasawebwidget.h"
#include "pluginsversion.h"

namespace KIPIPicasawebExportPlugin
{

PicasawebWindow::PicasawebWindow(KIPI::Interface* interface, const QString &tmpFolder, QWidget* /*parent*/)
               : KDialog(0), m_tmp(tmpFolder)
{
    setWindowTitle(i18n("Export to Picasa Web Service"));
    setWindowIcon(KIcon("picasa"));
    setModal(false);
    setButtons(Help|Close);
    setDefaultButton(Close);
    m_interface              = interface;
    m_uploadCount            = 0;
    m_uploadTotal            = 0;
//  m_wallet                 = 0;
    m_urls                   = 0;
    m_widget                 = new PicasawebWidget(this);
    m_photoView              = m_widget->m_photoView;
    m_newAlbumButton         = m_widget->m_newAlbumButton;
    m_addPhotoButton         = m_widget->m_selectPhotosButton;
    m_albumsListComboBox     = m_widget->m_albumsListComboBox;
    m_dimensionSpinBox       = m_widget->m_dimensionSpinBox;
    m_imageQualitySpinBox    = m_widget->m_imageQualitySpinBox;
    m_resizeCheckBox         = m_widget->m_resizeCheckBox;
    m_tagsLineEdit           = m_widget->m_tagsLineEdit;
    m_exportApplicationTags  = m_widget->m_exportApplicationTags;
    m_startUploadButton      = m_widget->m_startUploadButton;
    m_changeUserButton       = m_widget->m_changeUserButton;
    m_userNameDisplayLabel   = m_widget->m_userNameDisplayLabel;
    m_reloadAlbumsListButton = m_widget->m_reloadAlbumsListButton;

    setMainWidget(m_widget);
    m_widget->setMinimumSize(620, 300);
    m_widget->m_currentSelectionButton->setChecked(true);

    if(!m_interface->hasFeature(KIPI::HostSupportsTags))
        m_exportApplicationTags->setEnabled(false);

    // ------------------------------------------------------------

    m_about = new KIPIPlugins::KPAboutData(ki18n("PicasaWeb Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to export image collections to "
                                                 "PicasaWeb web service."),
                                           ki18n( "(c) 2007-2009, Vardhman Jain\n"
                                           "(c) 2008-2009, Gilles Caulier\n"
                                           "(c) 2009, Luka Renko" ));

    m_about->addAuthor(ki18n( "Vardhman Jain" ), ki18n("Author and maintainer"),
                       "Vardhman at gmail dot com");

    m_about->addAuthor(ki18n( "Gilles Caulier" ), ki18n("Developer"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n( "Luka Renko" ), ki18n("Developer"),
                       "lure at kubuntu dot org");

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)), 
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ------------------------------------------------------------

    m_talker = new PicasawebTalker(this);

    connect(m_talker, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy( bool)));

    connect(m_talker, SIGNAL(signalAddPhotoSucceeded()),
            this, SLOT(slotAddPhotoSucceeded()));

    connect(m_talker, SIGNAL(signalGetAlbumsListSucceeded()),
            this, SLOT(slotGetAlbumsListSucceeded()));

    connect(m_talker, SIGNAL(signalGetAlbumsListFailed(const QString&)),
            this, SLOT(slotGetAlbumsListFailed(const QString&)));

    connect(m_talker, SIGNAL( signalAddPhotoFailed(const QString&)),
            this, SLOT(slotAddPhotoFailed(const QString&)));

    // ------------------------------------------------------------

    m_progressDlg = new QProgressDialog(this);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);

    connect(m_progressDlg, SIGNAL(canceled()),
            this, SLOT(slotAddPhotoCancel()));

    connect(m_changeUserButton, SIGNAL(clicked()),
            this, SLOT(slotUserChangeRequest()));

    connect(m_reloadAlbumsListButton, SIGNAL(clicked()),
            this, SLOT(slotUpdateAlbumsList()));

    connect(m_newAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotCreateNewAlbum()));

    connect(m_talker, SIGNAL(signalTokenObtained(const QString&)),
            this, SLOT(slotTokenObtained(const QString&)));

    connect(m_addPhotoButton, SIGNAL(clicked()),
            this, SLOT(slotAddPhotos()));

    connect(m_startUploadButton, SIGNAL(clicked()),
            this, SLOT(slotUploadImages()));

    connect(m_resizeCheckBox, SIGNAL(toggled(bool )),
            this, SLOT(slotRefreshSizeButtons(bool)));

    // ------------------------------------------------------------
    // read config

    KConfig config("kipirc");
    KConfigGroup grp = config.group( "PicasawebExport Settings");
    QString token    = grp.readEntry("token");
    QString username = grp.readEntry("username");
    QString password = grp.readEntry("password");
    kDebug() << "Read token from database to be " << token ;

    //no saving password rt now
    if (grp.readEntry("Resize", false))
        m_resizeCheckBox->setChecked(true);

    m_dimensionSpinBox->setValue(grp.readEntry("Maximum Width", 1600));
    m_imageQualitySpinBox->setValue(grp.readEntry("Image Quality", 85));

    // ------------------------------------------------------------

    m_authProgressDlg = new QProgressDialog(this);
    m_authProgressDlg->setModal(true);
    m_authProgressDlg->setAutoReset( true );
    m_authProgressDlg->setAutoClose( true );

    connect(m_authProgressDlg, SIGNAL(canceled()),
            this, SLOT(slotAuthCancel()));

    m_talker->authProgressDlg = m_authProgressDlg;
    m_widget->setEnabled(false);
    m_talker->authenticate(token, username, password);
}

void PicasawebWindow::slotClose()
{
    kDebug() << "Writing token value as ########### " << m_talker->token() << " #######" ;
    saveSettings();
    delete m_urls;
    done(Close);
}

void PicasawebWindow::closeEvent(QCloseEvent *e) 
{
    if (!e) return;

    kDebug() << "Writing token value as ########### " << m_talker->token() << " #######" ;
    saveSettings();
    delete m_urls;
    e->accept();
}

void PicasawebWindow::slotRefreshSizeButtons(bool /*st*/)
{
    if(m_resizeCheckBox->isChecked())
    {
        m_dimensionSpinBox->setEnabled(true);
        m_imageQualitySpinBox->setEnabled(true);
    }
    else
    {
        m_dimensionSpinBox->setEnabled(false);
        m_imageQualitySpinBox->setEnabled(false);
    }
}

void PicasawebWindow::slotUpdateAlbumsList()
{
    m_talker->listAllAlbums();
}

void PicasawebWindow::saveSettings() 
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("PicasawebExport Settings");
    kDebug() << "Writing token value as ########### " << m_talker->token() << " #######" ;
    grp.writeEntry("token",         m_talker->token());
    grp.writeEntry("username",      m_username);
    grp.writeEntry("Resize",        m_resizeCheckBox->isChecked());
    grp.writeEntry("Maximum Width", m_dimensionSpinBox->value());
    grp.writeEntry("Image Quality", m_imageQualitySpinBox->value());
}

PicasawebWindow::~PicasawebWindow()
{
    delete m_urls;
    delete m_progressDlg;
    delete m_authProgressDlg;
    delete m_talker;
    delete m_widget;
    delete m_about;
}

void PicasawebWindow::getToken(QString& username, QString& password)
{
    QPointer<PicasawebLogin> loginDialog = new PicasawebLogin(this, i18n("Login"), username, password);
    if (!loginDialog)
    {
        return;
    }

    QString username_edit, password_edit;

    if (loginDialog->exec() == QDialog::Accepted)
    {
        username_edit = loginDialog->username();
        password_edit = loginDialog->password();
        delete loginDialog;
    }
    else
    {
        //Return something which say authentication needed.
        delete loginDialog;
        return ;
    }
}

void PicasawebWindow::slotHelp()
{
    KToolInvocation::invokeHelp("picasawebexport", "kipi-plugins");
}

void PicasawebWindow::slotGetAlbumsListSucceeded()
{
    m_albumsListComboBox->clear();
    if (m_talker && m_talker->m_albumsList)
    {
        QLinkedList <PicasaWebAlbum> *list       = m_talker->m_albumsList;
        QLinkedList<PicasaWebAlbum>::iterator it = list->begin();
        while(it != list->end())
        {
            PicasaWebAlbum pwa = *it;
            m_albumsListComboBox->addItem(pwa.title, pwa.id);
            it++;
        }
    }
}

void PicasawebWindow::slotDoLogin()
{
}

void PicasawebWindow::slotTokenObtained( const QString& /*token*/ )
{
    m_username = m_talker->getUserName();
    m_userId   = m_talker->getUserId();
    m_userNameDisplayLabel->setText(m_username);
    m_widget->setEnabled(true);
    m_talker->listAllAlbums();
}

void PicasawebWindow::slotBusy( bool val )
{
    if ( val )
    {
        setCursor(Qt::WaitCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void PicasawebWindow::slotError( const QString& msg )
{
    KMessageBox::error(this, msg);
}

void PicasawebWindow::slotUserChangeRequest()
{
    kDebug() << "Slot Change User Request ";
    m_talker->authenticate();
}

void PicasawebWindow::slotAuthCancel()
{
    m_talker->cancel();
    m_authProgressDlg->hide();
}

void PicasawebWindow::slotCreateNewAlbum()
{
    QPointer<NewAlbumDialog> dlg = new NewAlbumDialog(kapp->activeWindow());
    dlg->m_dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    QString access;
    int t = dlg->exec();

    if(t == QDialog::Accepted)
    {
        if (dlg->m_isPublicRadioButton->isChecked())
            access = QString("public");
        else if (dlg->m_isProtectedRadioButton->isChecked())
            access = QString("protected");
        else
            access = QString("private");

        long long timestamp = dlg->m_dateTimeEdit->dateTime().toTime_t();
        m_talker->createAlbum(dlg->m_titleLineEdit->text(), dlg->m_descriptionTextBox->toPlainText(),
                              dlg->m_locationLineEdit->text(), timestamp * 1000,
                              access, QString(), true);
    }
    else
    {
        if (t == QDialog::Rejected)
        {
            kDebug() << "Album Creation cancelled" ;
        }
    }

    delete dlg;
}

/*
void PicasawebWindow::slotPhotos( const QValueList<GPhoto>& photoList)
{
    // TODO
}

void PicasawebWindow::slotTagSelected()
{
    // TODO
}

void PicasawebWindow::slotOpenPhoto( const KUrl& url )
{
    new KRun(url);
}
*/

void PicasawebWindow::slotListPhotoSetsResponse(const QLinkedList <FPhotoSet>& /*photoSetList*/)
{
}

void PicasawebWindow::slotAddPhotos()
{
    //m_talker->listPhotoSets();
    KIPIPlugins::ImageDialog dlg(this, m_interface);
    delete m_urls;
    m_urls = new KUrl::List( dlg.urls() );
}

void PicasawebWindow::slotUploadImages()
{
    if (m_albumsListComboBox->currentIndex() == -1) 
    {
        KMessageBox::error(this, i18n("No album selected - please create and select album."));
        return;
    }
    m_currentAlbumId = m_albumsListComboBox->itemData(m_albumsListComboBox->currentIndex()).toString();

    if (m_widget->m_currentSelectionButton->isChecked())
    {
        delete m_urls;

        m_urls = new KUrl::List(m_interface->currentSelection().images());
    }

    if (m_urls == NULL || m_urls->isEmpty()) 
    {
        KMessageBox::error(this, i18n("Nothing to upload - please select photos to upload."));
        return;
    }

    typedef QPair<QString,FPhotoInfo> Pair;

    m_uploadQueue.clear();

    for (KUrl::List::ConstIterator it = m_urls->constBegin(); it != m_urls->constEnd(); ++it)
    {
        KIPI::ImageInfo info = m_interface->info( *it );
        FPhotoInfo temp;

        temp.title=info.title();
        temp.description=info.description();

        QStringList allTags;

        QStringList tagsFromDialog = m_tagsLineEdit->text().split(" ", QString::SkipEmptyParts);
        QStringList::Iterator itTags;

        // Tags from the interface
        itTags = tagsFromDialog.begin();

        while( itTags != tagsFromDialog.end() )
        {
            allTags.append( *itTags );
            ++itTags;
        }

        //Tags from the database
        QMap <QString, QVariant> attribs = info.attributes();
        QStringList tagsFromDatabase;

        if(m_exportApplicationTags->isChecked())
        {
            // tagsFromDatabase=attribs["tags"].asStringList();
        }

        itTags = tagsFromDatabase.begin();

        while( itTags != tagsFromDatabase.end() )
        {
            allTags.append( *itTags );
            ++itTags;
        }

        itTags = allTags.begin();

        while( itTags != allTags.end() )
        {
            ++itTags;
        }

        temp.tags=allTags;
        m_uploadQueue.append( Pair( (*it).path(), temp) );
    }

    m_uploadTotal = m_uploadQueue.count();
    m_uploadCount = 0;
    m_progressDlg->reset();
    slotAddPhotoNext();
}

void PicasawebWindow::slotAddPhotoNext()
{
    if ( m_uploadQueue.isEmpty() )
    {
        // m_progressDlg->reset();
        m_progressDlg->hide();
        //slotAlbumSelected();
        return;
    }

    typedef QPair<QString,FPhotoInfo> Pair;
    Pair pathComments = m_uploadQueue.first();
    FPhotoInfo info   = pathComments.second;
    m_uploadQueue.pop_front();

/*
    int upload_image_size;
    int upload_image_quality;
*/

    bool res = m_talker->addPhoto(pathComments.first,          //the file path
                                  info, m_currentAlbumId,
                                  m_resizeCheckBox->isChecked(),
                                  m_dimensionSpinBox->value(), m_imageQualitySpinBox->value());
    if (!res)
    {
        slotAddPhotoFailed("");
        return;
    }

    m_progressDlg->setLabelText(i18n("Uploading file %1 ", KUrl(pathComments.first).fileName()));

    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void PicasawebWindow::slotAddPhotoSucceeded()
{
    m_uploadCount++;
    m_progressDlg->setMaximum(m_uploadTotal);
    m_progressDlg->setValue(m_uploadCount);
    slotAddPhotoNext();
}

void PicasawebWindow::slotAddPhotoFailed(const QString& msg)
{
    if ( KMessageBox::warningContinueCancel(this,
           i18n("Failed to upload photo to PicasaWeb. %1\nDo you want to continue?",
                msg )) != KMessageBox::Continue)
    {
        m_uploadQueue.clear();
        m_progressDlg->reset();
        m_progressDlg->hide();
        // refresh the thumbnails
        //slotTagSelected();
    }
    else
    {
        m_uploadTotal--;
        m_progressDlg->setMaximum( m_uploadTotal);
        m_progressDlg->setValue(m_uploadCount);
        slotAddPhotoNext();
    }
}

void PicasawebWindow::slotGetAlbumsListFailed(const QString& /*msg*/)
{
    // Raise some errors
    m_albumsListComboBox->clear();
}

void PicasawebWindow::slotAddPhotoCancel()
{
    m_uploadQueue.clear();
    m_progressDlg->reset();
    m_progressDlg->hide();
    m_talker->cancel();

    // refresh the thumbnails
    //slotTagSelected();
}

} // namespace KIPIPicasawebExportPlugin
