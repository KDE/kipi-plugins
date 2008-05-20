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

// Qt includes.

#include <qlistview.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <qprogressdialog.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qstringlist.h>
#include <qradiobutton.h>

// KDE includes.

#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <krun.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kdeversion.h>
#if KDE_IS_VERSION(3,2,0)
#include <kwallet.h>
#endif

// Libkipi includes.

#include <libkipi/interface.h>
#include <libkipi/imagedialog.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "flickrtalker.h"
#include "flickritem.h"
#include "flickrviewitem.h"
#include "login.h"
#include "flickrwidget.h"
#include "flickralbumdialog.h"
#include "flickrwindow.h"
#include "flickrwindow.moc"

namespace KIPIFlickrExportPlugin
{

FlickrWindow::FlickrWindow(KIPI::Interface* interface,const QString &tmpFolder, QWidget *parent)
            : KDialogBase(0, 0, false, i18n("Flickr Export"), Help|Close, Close, false)
{
    m_tmp         = tmpFolder;
    m_interface   = interface;
    m_uploadCount = 0;
    m_uploadTotal = 0;
//  m_wallet      = 0;

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Flickr Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to export image collection to "
                                                     "Flickr web service."),
                                           "(c) 2005-2008, Vardhman Jain\n"
                                           "(c) 2008, Gilles Caulier");

    m_about->addAuthor("Vardhman Jain", I18N_NOOP("Author and maintainer"),
                       "Vardhman at gmail dot com");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Developer"),
                       "caulier dot gilles at gmail dot com");

    m_helpButton        = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup(helpMenu->menu());
    m_widget = new FlickrWidget(this);
    setMainWidget(m_widget);
    m_widget->setMinimumSize(600, 400);

    m_urls                  = NULL;
    m_tagView               = m_widget->m_tagView;
    m_photoView             = m_widget->m_photoView;
    //m_newAlbumBtn           = widget->m_newAlbumBtn;
    m_addPhotoBtn           = m_widget->m_addPhotoBtn;
    m_resizeCheckBox        = m_widget->m_resizeCheckBox;
    m_publicCheckBox	    = m_widget->m_publicCheckBox;
    m_familyCheckBox        = m_widget->m_familyCheckBox;
    m_friendsCheckBox       = m_widget->m_friendsCheckBox;
    m_dimensionSpinBox      = m_widget->m_dimensionSpinBox;
    m_imageQualitySpinBox   = m_widget->m_imageQualitySpinBox;
    m_tagsLineEdit          = m_widget->m_tagsLineEdit;
    m_exportApplicationTags = m_widget->m_exportApplicationTags;
    m_startUploadButton     = m_widget->m_startUploadButton;
    m_changeUserButton      = m_widget->m_changeUserButton;
    m_userNameDisplayLabel  = m_widget->m_userNameDisplayLabel;

    //m_startUploadButton->setEnabled(false);
    //m_albumView->setRootIsDecorated( true );
    //m_newAlbumBtn->setEnabled( false );
    m_addPhotoBtn->setEnabled(false);
    //if(!m_interface->hasFeature(KIPI::HostSupportsTags))
    //    m_exportApplicationTags->setEnabled(false);

    m_talker = new FlickrTalker(this);

    connect(m_talker, SIGNAL( signalError( const QString& ) ),
            m_talker, SLOT( slotError( const QString& ) ));

    connect(m_talker, SIGNAL( signalBusy( bool ) ),
            this, SLOT( slotBusy( bool ) ));

    //connect( m_talker, SIGNAL( signalAlbums( const QValueList<GAlbum>& ) ),
    //         SLOT( slotAlbums( const QValueList<GAlbum>& ) ) );

    //connect( m_talker, SIGNAL( signalPhotos( const QValueList<GPhoto>& ) ),
    //         SLOT( slotPhotos( const QValueList<GPhoto>& ) ) );

    connect(m_talker, SIGNAL( signalAddPhotoSucceeded() ),
            this, SLOT( slotAddPhotoSucceeded() ));

    connect(m_talker, SIGNAL( signalAddPhotoFailed( const QString& ) ),
            this, SLOT( slotAddPhotoFailed( const QString& ) ));

    connect(m_talker, SIGNAL( signalListPhotoSetsSucceeded( const QValueList<FPhotoSet>& ) ),
            this, SLOT( slotListPhotoSetsResponse( const QValueList<FPhotoSet>& ) ));

    m_progressDlg = new QProgressDialog(this, 0, true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);

    connect(m_progressDlg, SIGNAL( canceled() ),
            this, SLOT( slotAddPhotoCancel() ));

    connect(m_changeUserButton, SIGNAL( clicked() ),
            this, SLOT( slotUserChangeRequest() ));

    //connect( m_tagView, SIGNAL( selectionChanged() ),
    //         SLOT( slotTagSelected() ) );

    connect(m_talker, SIGNAL( signalTokenObtained(const QString&) ),
            this, SLOT( slotTokenObtained(const QString&) ));

    //connect( m_photoView->browserExtension(), SIGNAL( openURLRequest( const KURL&, const KParts::URLArgs& ) ),
    //         SLOT( slotOpenPhoto( const KURL& ) ) );

    //connect( m_newAlbumBtn, SIGNAL( clicked() ),
    //         SLOT( slotNewAlbum() ) );

    connect(m_addPhotoBtn, SIGNAL( clicked() ),
            this, SLOT( slotAddPhotos() ));

    connect(m_startUploadButton, SIGNAL( clicked() ),
            this, SLOT( slotUploadImages() ));

    // read config
    KConfig config("kipirc");
    config.setGroup("FlickrExport Settings");
    m_token = config.readEntry("token");
    if (config.readBoolEntry("Resize", false))
    {
        m_resizeCheckBox->setChecked(true);
        m_dimensionSpinBox->setEnabled(true);
    }
    else
    {
        m_resizeCheckBox->setChecked(false);
        m_dimensionSpinBox->setEnabled(false);
    }
    m_dimensionSpinBox->setValue(config.readNumEntry("Maximum Width", 1600));
    m_imageQualitySpinBox->setValue(config.readNumEntry("Image Quality", 85));

    m_authProgressDlg = new QProgressDialog(this, 0, true);
    m_authProgressDlg->setAutoReset(true);
    m_authProgressDlg->setAutoClose(true);

    connect(m_authProgressDlg, SIGNAL( canceled() ),
            this, SLOT( slotAuthCancel() ));

    m_talker->authProgressDlg=m_authProgressDlg; 
    m_widget->setEnabled(false); 

    kdDebug() << "Calling auth methods" << endl; 

    if(m_token.length()< 1)  
        m_talker->getFrob();
    else
        m_talker->checkToken(m_token);
}

FlickrWindow::~FlickrWindow()
{
#if KDE_IS_VERSION(3,2,0)
//   if (m_wallet)
//      delete m_wallet;
#endif

    // write config
    KConfig config("kipirc");
    config.setGroup("FlickrExport Settings");
    config.writeEntry("token", m_token);
    config.writeEntry("Resize", m_resizeCheckBox->isChecked());
    config.writeEntry("Maximum Width",  m_dimensionSpinBox->value());
    config.writeEntry("Image Quality",  m_imageQualitySpinBox->value());

    if(m_urls!=NULL)
        delete m_urls;

    delete m_progressDlg;
    delete m_authProgressDlg;
    delete m_talker;
    delete m_widget;
    delete m_about;
}

void FlickrWindow::slotHelp()
{
    KApplication::kApplication()->invokeHelp("flickrexport", "kipi-plugins");
}

void FlickrWindow::slotDoLogin()
{
}

void FlickrWindow::slotClose()
{
    delete this;
}

void FlickrWindow::slotTokenObtained( const QString& token )
{
    m_token=token;
    m_username=m_talker->getUserName();
    m_userId=m_talker->getUserId();
    kdDebug() << "SlotTokenObtained invoked setting user Display name to " << m_username << endl;
    m_userNameDisplayLabel->setText(m_username);
    m_widget->setEnabled(true);
}

void FlickrWindow::slotBusy(bool val)
{
    if ( val )
    {
        setCursor(QCursor::WaitCursor);
//      m_newAlbumBtn->setEnabled( false );
//      m_addPhotoBtn->setEnabled( false );
    }
    else
    {
        setCursor(QCursor::ArrowCursor);
//      m_newAlbumBtn->setEnabled( loggedIn );
//      m_addPhotoBtn->setEnabled( loggedIn && m_albumView->selectedItem() );
    }
}

void FlickrWindow::slotError( const QString& msg )
{
    //m_talker->slotError(msg);
    KMessageBox::error( this, msg );
}

void FlickrWindow::slotUserChangeRequest()
{
    kdDebug()<<"Slot Change User Request "<<endl;
    m_talker->getFrob();
    //m_addPhotoBtn->setEnabled(m_selectImagesButton->isChecked());
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
    QPixmap pix = iconLoader->loadIcon( "folder", KIcon::NoGroup, 32 );

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
                kdWarning() << "Failed to find parent for album "
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
void FlickrWindow::slotOpenPhoto( const KURL& url )
{
    new KRun(url);
}
*/

void FlickrWindow::slotListPhotoSetsResponse(const QValueList <FPhotoSet>& photoSetList)
{
   kdDebug() << "SlotListPhotoSetsResponse invoked" << endl;
}

void FlickrWindow::slotNewPhotoSet()
{
    // TODO
}

void FlickrWindow::slotAddPhotos()
{
    kdDebug() << "Slot Add Photos called geting the list of url" << endl;
    //m_talker->listPhotoSets();
    m_urls = new KURL::List(KIPI::ImageDialog::getImageURLs(this, m_interface));
}

void FlickrWindow::slotUploadImages()
{
    kdDebug() << "SlotUploadImages invoked" << endl;

    if(m_widget->m_currentSelectionButton->isChecked())
    {
        kdDebug() << "Using Selection" << endl;

        if (m_urls != NULL)
            delete m_urls;

        m_urls = new KURL::List(m_interface->currentSelection().images());
    }

    if (m_urls == NULL || m_urls->isEmpty())
        return;

    typedef QPair<QString,FPhotoInfo> Pair;

    m_uploadQueue.clear();

    for (KURL::List::iterator it = m_urls->begin(); it != m_urls->end(); ++it)
    {
        KIPI::ImageInfo info = m_interface->info( *it );
        kdDebug() <<" Adding images to the list" << endl;
        FPhotoInfo temp;

        temp.title       = info.title();
        temp.description = info.description();
        temp.is_public   = m_publicCheckBox->isChecked();
        temp.is_family   = m_familyCheckBox->isChecked();
        temp.is_friend   = m_friendsCheckBox->isChecked();

        QStringList allTags;

        QStringList tagsFromDialog = QStringList::split(" ",m_tagsLineEdit->text(),false); 
        QStringList::Iterator itTags;

        //Tags from the interface	
        itTags = tagsFromDialog.begin();
        while( itTags != tagsFromDialog.end() ) 
        {
            allTags.append( *itTags );
            ++itTags;
        }

        //Tags from the database
        QMap <QString, QVariant> attribs = info.attributes();
        QStringList tagsFromDatabase;

        if(/*m_interface->hasFeature(KIPI::HostSupportsTags) && */
             m_exportApplicationTags->isChecked())
        {
            tagsFromDatabase=attribs["tags"].asStringList();	
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
            kdDebug()<<"TAGS"<< ( *itTags )<<endl;
            ++itTags;
        }

        temp.tags = allTags; 
        m_uploadQueue.append( Pair( (*it).path(), temp) );
    }

    m_uploadTotal = m_uploadQueue.count();
    m_uploadCount = 0;
    m_progressDlg->reset();
    slotAddPhotoNext();
    kdDebug() << "SlotUploadImages done" << endl;
}

void FlickrWindow::slotAddPhotoNext()
{
    if ( m_uploadQueue.isEmpty() )
    {
        m_progressDlg->reset();
        m_progressDlg->hide();
        //slotAlbumSelected();
        return;
    }

    typedef QPair<QString,FPhotoInfo> Pair;
    Pair pathComments = m_uploadQueue.first();
    FPhotoInfo info   = pathComments.second;
    m_uploadQueue.pop_front();
    bool res = m_talker->addPhoto(pathComments.first, //the file path
                                  info,
                                  m_resizeCheckBox->isChecked(),
                                  m_dimensionSpinBox->value(),m_imageQualitySpinBox->value() );
    if (!res)
    {
        slotAddPhotoFailed( "" );
        return;
    }

    m_progressDlg->setLabelText(i18n("Uploading file %1 ")
                                .arg( KURL(pathComments.first).filename() ));

    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void FlickrWindow::slotAddPhotoSucceeded()
{
    m_uploadCount++;
    m_progressDlg->setProgress( m_uploadCount, m_uploadTotal );
    slotAddPhotoNext();
}

void FlickrWindow::slotAddPhotoFailed( const QString& msg )
{
    if (KMessageBox::warningContinueCancel(this,
                     i18n("Failed to upload photo into Flickr. %1\nDo you want to continue?").arg( msg )) 
                     != KMessageBox::Continue )
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
        m_progressDlg->setProgress( m_uploadCount, m_uploadTotal );
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

} // namespace KIPIFlickrExportPlugin
