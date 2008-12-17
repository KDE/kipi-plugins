/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "flickrwindow.h"
#include "flickrwindow.moc"

// Qt includes.

#include <QProgressDialog>
#include <QPushButton>
#include <QPixmap>
#include <QCheckBox>
#include <QStringList>
#include <QSpinBox>

// KDE includes.

#include <klineedit.h>
#include <khelpmenu.h>
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

// LibKIPI includes.

#include <libkipi/interface.h>
#include <ktoolinvocation.h>

// Local includes.

#include "imageslist.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "login.h"
#include "flickrtalker.h"
#include "flickritem.h"
#include "flickrviewitem.h"
#include "flickrwidget.h"
#include "ui_flickralbumdialog.h"

namespace KIPIFlickrExportPlugin
{

FlickrWindow::FlickrWindow(KIPI::Interface* interface, const QString &tmpFolder, QWidget *parent)
            : KDialog(parent)
{
    setWindowTitle(i18n("Export to Flickr Web Service"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);

    m_tmp                    = tmpFolder;
    m_interface              = interface;
    m_uploadCount            = 0;
    m_uploadTotal            = 0;
//  m_wallet                 = 0;
    m_widget                 = new FlickrWidget(this, interface);
    m_photoView              = m_widget->m_photoView;
//  m_newAlbumBtn            = m_widget->m_newAlbumBtn;
    m_resizeCheckBox         = m_widget->m_resizeCheckBox;
    m_publicCheckBox         = m_widget->m_publicCheckBox;
    m_familyCheckBox         = m_widget->m_familyCheckBox;
    m_friendsCheckBox        = m_widget->m_friendsCheckBox;
    m_dimensionSpinBox       = m_widget->m_dimensionSpinBox;
    m_imageQualitySpinBox    = m_widget->m_imageQualitySpinBox;
    m_tagsLineEdit           = m_widget->m_tagsLineEdit;
    m_exportHostTagsCheckBox = m_widget->m_exportHostTagsCheckBox;
    m_stripSpaceTagsCheckBox = m_widget->m_stripSpaceTagsCheckBox;
    m_changeUserButton       = m_widget->m_changeUserButton;
    m_userNameDisplayLabel   = m_widget->m_userNameDisplayLabel;
    m_imglst                 = m_widget->m_imglst;

    setButtonGuiItem(User1, KGuiItem(i18n("Start Uploading"), KIcon("network-workgroup")));
    setMainWidget(m_widget);
    m_widget->setMinimumSize(600, 400);

    connect(m_imglst, SIGNAL(signalImageListChanged(bool)),
            this, SLOT(slotImageListChanged(bool)));

    //m_startUploadButton->setEnabled(false);
    //m_albumView->setRootIsDecorated(true);
    //m_newAlbumBtn->setEnabled(false);

    // --------------------------------------------------------------------------
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Flickr Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to export image collection to "
                                                     "Flickr web service."),
                                           ki18n( "(c) 2005-2008, Vardhman Jain\n"
                                           "(c) 2008, Gilles Caulier" ));

    m_about->addAuthor(ki18n( "Vardhman Jain" ), ki18n("Author and maintainer"),
                       "Vardhman at gmail dot com");

    m_about->addAuthor(ki18n( "Gilles Caulier" ), ki18n("Developer"),
                       "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu* helpMenu     = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook       = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu( helpMenu->menu() );

    // --------------------------------------------------------------------------

    m_talker = new FlickrTalker(this);

    connect(m_talker, SIGNAL( signalError( const QString& ) ),
            m_talker, SLOT( slotError( const QString& ) ));

    connect(m_talker, SIGNAL( signalBusy( bool ) ),
            this, SLOT( slotBusy( bool ) ));

    connect(m_talker, SIGNAL( signalAddPhotoSucceeded() ),
            this, SLOT( slotAddPhotoSucceeded() ));

    connect(m_talker, SIGNAL( signalAddPhotoFailed( const QString& ) ),
            this, SLOT( slotAddPhotoFailed( const QString& ) ));

    connect(m_talker, SIGNAL( signalListPhotoSetsSucceeded( const QList<FPhotoSet>& ) ),
            this, SLOT( slotListPhotoSetsResponse( const QList<FPhotoSet>& ) ));

    //connect( m_talker, SIGNAL( signalAlbums( const QValueList<GAlbum>& ) ),
    //         SLOT( slotAlbums( const QValueList<GAlbum>& ) ) );

    //connect( m_talker, SIGNAL( signalPhotos( const QValueList<GPhoto>& ) ),
    //         SLOT( slotPhotos( const QValueList<GPhoto>& ) ) );

    // --------------------------------------------------------------------------

    m_progressDlg = new QProgressDialog(this);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);

    connect(m_progressDlg, SIGNAL( canceled() ),
            this, SLOT( slotAddPhotoCancel() ));

    connect(m_changeUserButton, SIGNAL( clicked() ),
            this, SLOT( slotUserChangeRequest() ));

    connect(m_talker, SIGNAL( signalTokenObtained(const QString&) ),
            this, SLOT( slotTokenObtained(const QString&) ));

    //connect( m_tagView, SIGNAL( selectionChanged() ),
    //         SLOT( slotTagSelected() ) );

    //connect( m_photoView->browserExtension(), SIGNAL( openURLRequest( const KUrl&, const KParts::URLArgs& ) ),
    //         SLOT( slotOpenPhoto( const KUrl& ) ) );

    //connect( m_newAlbumBtn, SIGNAL( clicked() ),
    //         SLOT( slotNewAlbum() ) );

    // --------------------------------------------------------------------------

    readSettings();

    m_authProgressDlg = new QProgressDialog(this);
    m_authProgressDlg->setModal(true);
    m_authProgressDlg->setAutoReset(true);
    m_authProgressDlg->setAutoClose(true);

    connect(m_authProgressDlg, SIGNAL( canceled() ),
            this, SLOT( slotAuthCancel() ));

    m_talker->m_authProgressDlg = m_authProgressDlg;
    m_widget->setEnabled(false);

    kDebug( 51000 ) << "Calling auth methods" << endl;

    if(m_token.length()< 1)
        m_talker->getFrob();
    else
        m_talker->checkToken(m_token);

    // --------------------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotUser1()));
}

FlickrWindow::~FlickrWindow()
{
#if KDE_IS_VERSION(3,2,0)
//   if (m_wallet)
//      delete m_wallet;
#endif

    delete m_progressDlg;
    delete m_authProgressDlg;
    delete m_talker;
    delete m_widget;
    delete m_about;
}

void FlickrWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("FlickrExport Settings");
    m_token = grp.readEntry("token");

    if (grp.readEntry("Resize", false))
    {
        m_resizeCheckBox->setChecked(true);
        m_dimensionSpinBox->setEnabled(true);
    }
    else
    {
        m_resizeCheckBox->setChecked(false);
        m_dimensionSpinBox->setEnabled(false);
    }

    m_dimensionSpinBox->setValue(grp.readEntry("Maximum Width", 1600));
    m_imageQualitySpinBox->setValue(grp.readEntry("Image Quality", 85));
    m_exportHostTagsCheckBox->setChecked(grp.readEntry("Export Host Tags", false));
    m_stripSpaceTagsCheckBox->setChecked(grp.readEntry("Strip Space Host Tags", false));
    m_stripSpaceTagsCheckBox->setEnabled(m_exportHostTagsCheckBox->isChecked());

    if(!m_interface->hasFeature(KIPI::HostSupportsTags))
    {
        m_exportHostTagsCheckBox->setEnabled(false);
        m_stripSpaceTagsCheckBox->setEnabled(false);
    }

    m_publicCheckBox->setChecked(grp.readEntry("Public Sharing", false));
    m_familyCheckBox->setChecked(grp.readEntry("Family Sharing", false));
    m_friendsCheckBox->setChecked(grp.readEntry("Friends Sharing", false));
    KConfigGroup dialogGroup = config.group( "FlickrExport Dialog");
    restoreDialogSize(dialogGroup);
}

void FlickrWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("FlickrExport Settings");
    grp.writeEntry("token", m_token);
    grp.writeEntry("Resize", m_resizeCheckBox->isChecked());
    grp.writeEntry("Maximum Width",  m_dimensionSpinBox->value());
    grp.writeEntry("Image Quality",  m_imageQualitySpinBox->value());
    grp.writeEntry("Export Host Tags", m_exportHostTagsCheckBox->isChecked());
    grp.writeEntry("Strip Space Host Tags", m_stripSpaceTagsCheckBox->isChecked());
    grp.writeEntry("Public Sharing", m_publicCheckBox->isChecked());
    grp.writeEntry("Family Sharing", m_familyCheckBox->isChecked());
    grp.writeEntry("Friends Sharing", m_friendsCheckBox->isChecked());
    KConfigGroup dialogGroup = config.group( "FlickrExport Dialog");
    saveDialogSize(dialogGroup );
    config.sync();
}

void FlickrWindow::slotHelp()
{
    KToolInvocation::invokeHelp("flickrexport", "kipi-plugins");
}

void FlickrWindow::slotDoLogin()
{
}

void FlickrWindow::slotClose()
{
    writeSettings();
    done(Close);
}

void FlickrWindow::slotTokenObtained(const QString& token)
{
    m_token    = token;
    m_username = m_talker->getUserName();
    m_userId   = m_talker->getUserId();
    kDebug( 51000 ) << "SlotTokenObtained invoked setting user Display name to " << m_username << endl;
    m_userNameDisplayLabel->setText(QString("<b>%1</b>").arg(m_username));
    m_widget->setEnabled(true);
}

void FlickrWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
//      m_newAlbumBtn->setEnabled( false );
//      m_addPhotoButton->setEnabled( false );
    }
    else
    {
        setCursor(Qt::ArrowCursor);
//      m_newAlbumBtn->setEnabled( loggedIn );
//      m_addPhotoButton->setEnabled( loggedIn && m_albumView->selectedItem() );
    }
}

void FlickrWindow::slotError(const QString& msg)
{
    //m_talker->slotError(msg);
    KMessageBox::error( this, msg );
}

void FlickrWindow::slotUserChangeRequest()
{
    kDebug( 51000 ) << "Slot Change User Request " << endl;
    m_talker->getFrob();
//  m_addPhotoButton->setEnabled(m_selectImagesButton->isChecked());
}

/*
void FlickrWindow::slotAlbums( const QValueList<GAlbum>& albumList )
{
    m_albumDict.clear();
    m_tagView->clear();
    //m_photoView->begin();
    //m_photoView->write( "<html></html>" );
    //m_photoView->end();

    KIconLoader* iconLoader = KApplication::kApplication()->iconLoader();
    QPixmap pix = iconLoader->loadIcon( "folder", KIconLoader::NoGroup, 32 );

    typedef QValueList<GAlbum> GAlbumList;
    GAlbumList::const_iterator iter;
    for ( iter = albumList.begin(); iter != albumList.end(); ++iter )
    {
        const GAlbum& album = *iter;

        if ( album.parent_ref_num == 0 )
        {
            GAlbumViewItem* item = new GAlbumViewItem( m_tagView, album.name,
                                                       album );
            item->setPixmap( 0, pix );
            m_albumDict.insert( album.ref_num, item );
        }
        else
        {
            QListViewItem* parent = m_albumDict.find( album.parent_ref_num );
            if ( parent )
            {
                GAlbumViewItem* item = new GAlbumViewItem( parent, album.name,
                                                           album);
                item->setPixmap( 0, pix );
                m_albumDict.insert( album.ref_num, item );
            }
            else
            {
                kWarning( 51000 ) << "Failed to find parent for album "
                            << album.name
                            << "with id " << album.ref_num;
            }
        }
    }


    // find and select the last selected album
    int lastSelectedID = 0;
    for ( iter = albumList.begin(); iter != albumList.end(); ++iter )
    {
        if ((*iter).name == m_lastSelectedAlbum)
        {
            lastSelectedID = (*iter).ref_num;
            break;
        }
    }

    if (lastSelectedID > 0)
    {
        GAlbumViewItem* lastSelectedItem = m_albumDict.find( lastSelectedID );
        if (lastSelectedItem)
        {
            m_tagView->setSelected( lastSelectedItem, true );
            m_tagView->ensureItemVisible( lastSelectedItem );
        }
    }
}
*/

void FlickrWindow::slotAuthCancel()
{
    m_talker->cancel();
    m_authProgressDlg->hide();
}

/*
void FlickrWindow::slotPhotos( const QValueList<GPhoto>& photoList)
{
    // TODO
}

void FlickrWindow::slotTagSelected()
{
    // TODO
}
void FlickrWindow::slotOpenPhoto( const KUrl& url )
{
    new KRun(url);
}
*/

void FlickrWindow::slotListPhotoSetsResponse(const QList <FPhotoSet>& /*photoSetList*/)
{
    kDebug( 51000 ) << "SlotListPhotoSetsResponse invoked" << endl;
    // TODO
}

void FlickrWindow::slotNewPhotoSet()
{
    // TODO
}

/** This slot is call when 'Start Uploading' button is pressed.
*/
void FlickrWindow::slotUser1()
{
    kDebug( 51000 ) << "SlotUploadImages invoked" << endl;

    m_widget->m_tab->setCurrentIndex(FlickrWidget::FILELIST);
    KUrl::List urls = m_imglst->imageUrls();

    if (urls.isEmpty())
        return;

    typedef QPair<KUrl, FPhotoInfo> Pair;

    m_uploadQueue.clear();

    for (KUrl::List::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        KIPI::ImageInfo info = m_interface->info(*it);
        kDebug( 51000 ) << "Adding images to the list" << endl;
        FPhotoInfo temp;

        temp.title                 = info.title();
        temp.description           = info.description();
        temp.is_public             = m_publicCheckBox->isChecked();
        temp.is_family             = m_familyCheckBox->isChecked();
        temp.is_friend             = m_friendsCheckBox->isChecked();
        QStringList tagsFromDialog = m_tagsLineEdit->text().split(" ", QString::SkipEmptyParts);

        QStringList           allTags;
        QStringList::Iterator itTags;

        // Tags from the dialog
        itTags = tagsFromDialog.begin();
        while(itTags != tagsFromDialog.end())
        {
            allTags.append(*itTags);
            ++itTags;
        }

        // Tags from the database
        QMap <QString, QVariant> attribs = info.attributes();
        QStringList tagsFromDatabase;

        if(m_exportHostTagsCheckBox->isChecked())
        {
            tagsFromDatabase = attribs["tags"].toStringList();
            if (m_stripSpaceTagsCheckBox->isChecked())
            {
                for (QStringList::iterator it = tagsFromDatabase.begin(); it != tagsFromDatabase.end() ; ++it)
                    *it = (*it).trimmed().remove(" ");
            }
        }

        itTags = tagsFromDatabase.begin();

        while(itTags != tagsFromDatabase.end())
        {
            allTags.append(*itTags);
            ++itTags;
        }

        itTags = allTags.begin();

        while(itTags != allTags.end())
        {
            kDebug( 51000 ) << "Tags list: " << (*itTags) << endl;
            ++itTags;
        }

        temp.tags = allTags;
        m_uploadQueue.append(Pair(*it, temp));
    }

    m_uploadTotal = m_uploadQueue.count();
    m_uploadCount = 0;
    m_progressDlg->reset();
    slotAddPhotoNext();
    kDebug( 51000 ) << "SlotUploadImages done" << endl;
}

void FlickrWindow::slotAddPhotoNext()
{
    if (m_uploadQueue.isEmpty())
    {
        m_progressDlg->reset();
        m_progressDlg->hide();
        //slotAlbumSelected();
        return;
    }

    typedef QPair<KUrl, FPhotoInfo> Pair;
    Pair pathComments = m_uploadQueue.first();
    FPhotoInfo info   = pathComments.second;
    bool res          = m_talker->addPhoto(pathComments.first.path(), //the file path
                                           info,
                                           m_resizeCheckBox->isChecked(),
                                           m_dimensionSpinBox->value(),
                                           m_imageQualitySpinBox->value());
    if (!res)
    {
        slotAddPhotoFailed("");
        return;
    }

    m_progressDlg->setLabelText(i18n("Uploading file %1",pathComments.first.fileName()));

    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void FlickrWindow::slotAddPhotoSucceeded()
{
    // Remove photo uploaded from the list
    m_imglst->removeItemByUrl(m_uploadQueue.first().first);
    m_uploadQueue.pop_front();
    m_uploadCount++;
    m_progressDlg->setMaximum(m_uploadTotal);
    m_progressDlg->setValue(m_uploadCount);
    slotAddPhotoNext();
}

void FlickrWindow::slotAddPhotoFailed(const QString& msg)
{
    if (KMessageBox::warningContinueCancel(this,
                     i18n("Failed to upload photo into Flickr. %1\nDo you want to continue?", msg))
                     != KMessageBox::Continue)
    {
        m_uploadQueue.clear();
        m_progressDlg->reset();
        m_progressDlg->hide();
        // refresh the thumbnails
        //slotTagSelected();
    }
    else
    {
        m_uploadQueue.pop_front();
        m_uploadTotal--;
        m_progressDlg->setMaximum(m_uploadTotal);
        m_progressDlg->setValue(m_uploadCount);
        slotAddPhotoNext();
    }
}

void FlickrWindow::slotAddPhotoCancel()
{
    m_uploadQueue.clear();
    m_progressDlg->reset();
    m_progressDlg->hide();

    m_talker->cancel();

    // refresh the thumbnails
    //slotTagSelected();
}

void FlickrWindow::slotImageListChanged(bool state)
{
    enableButton(User1, !state);
}

} // namespace KIPIFlickrExportPlugin
