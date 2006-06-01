/* ============================================================
 * File  : gallerywindow.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Description :
 *
 * Copyright 2004-2005 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Include files for Qt

#include <qlistview.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <qprogressdialog.h>
#include <qspinbox.h>
#include <qcheckbox.h>

// Include files for KDE

#include <kaboutdata.h>
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

// KIPI include files

#include <libkipi/version.h>
#include <libkipi/interface.h>
#include <libkipi/imagedialog.h>

// Local includes.

#include "gallerytalker.h"
#include "galleryitem.h"
#include "galleryviewitem.h"
#include "gallerylogin.h"
#include "gallerywidget.h"
#include "galleryalbumdialog.h"
#include "gallerywindow.h"

namespace KIPIGalleryExportPlugin
{

GalleryWindow::GalleryWindow(KIPI::Interface* interface, QWidget *parent)
    : KDialogBase(parent, 0, true, i18n( "Gallery Export" ), Help|Close, Close, false)
{
    m_interface   = interface;
    m_uploadCount = 0;
    m_uploadTotal = 0;
    m_wallet      = 0;

    // About data and help button.

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Gallery Export"),
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin to export image collection to remote Gallery server."),
                                       KAboutData::License_GPL,
                                       "(c) 2004-2005, Renchi Raju",
                                       0,
                                       "http://extragear.kde.org/apps/kipi");

    about->addAuthor("Renchi Raju", I18N_NOOP("Author and maintainer"),
                     "renchi at pooh dot tam dot uiuc dot edu");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Gallery Export Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    GalleryWidget* widget = new GalleryWidget( this );
    setMainWidget( widget );
    widget->setMinimumSize( 600, 400 );

    m_albumView        = widget->m_albumView;
    m_photoView        = widget->m_photoView;
    m_newAlbumBtn      = widget->m_newAlbumBtn;
    m_addPhotoBtn      = widget->m_addPhotoBtn;
    m_resizeCheckBox   = widget->m_resizeCheckBox;
    m_dimensionSpinBox = widget->m_dimensionSpinBox;

    m_albumView->setRootIsDecorated( true );

    m_newAlbumBtn->setEnabled( false );
    m_addPhotoBtn->setEnabled( false );

    m_progressDlg = new QProgressDialog( this, 0, true );
    m_progressDlg->setAutoReset( true );
    m_progressDlg->setAutoClose( true );

    connect( m_progressDlg, SIGNAL( canceled() ),
             SLOT( slotAddPhotoCancel() ) );

    connect( m_albumView, SIGNAL( selectionChanged() ),
             SLOT( slotAlbumSelected() ) );
    connect( m_photoView->browserExtension(),
             SIGNAL( openURLRequest( const KURL&,
                                     const KParts::URLArgs& ) ),
             SLOT( slotOpenPhoto( const KURL& ) ) );

    connect( m_newAlbumBtn, SIGNAL( clicked() ),
             SLOT( slotNewAlbum() ) );
    connect( m_addPhotoBtn, SIGNAL( clicked() ),
             SLOT( slotAddPhotos() ) );

    // read config
    KConfig config("kipirc");
    config.setGroup("GalleryExport Settings");
    m_url  = config.readEntry("URL");
    m_user = config.readEntry("User");
    GalleryTalker::setGallery2(config.readBoolEntry("Gallery2", true));

    m_talker = new GalleryTalker( this );
    connect( m_talker, SIGNAL( signalError( const QString& ) ),
             SLOT( slotError( const QString& ) ) );
    connect( m_talker, SIGNAL( signalBusy( bool ) ),
             SLOT( slotBusy( bool ) ) );
    connect( m_talker,  SIGNAL( signalLoginFailed( const QString& ) ),
             SLOT( slotLoginFailed( const QString& ) ) );
    connect( m_talker, SIGNAL( signalAlbums( const QValueList<GAlbum>& ) ),
             SLOT( slotAlbums( const QValueList<GAlbum>& ) ) );
    connect( m_talker, SIGNAL( signalPhotos( const QValueList<GPhoto>& ) ),
             SLOT( slotPhotos( const QValueList<GPhoto>& ) ) );
    connect( m_talker, SIGNAL( signalAddPhotoSucceeded() ),
             SLOT( slotAddPhotoSucceeded() ) );
    connect( m_talker, SIGNAL( signalAddPhotoFailed( const QString& ) ),
             SLOT( slotAddPhotoFailed( const QString& ) ) );

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

    QTimer::singleShot( 0, this,  SLOT( slotDoLogin() ) );
}

GalleryWindow::~GalleryWindow()
{
#if KDE_IS_VERSION(3,2,0)
    if (m_wallet)
        delete m_wallet;
#endif

    // write config
    KConfig config("kipirc");
    config.setGroup("GalleryExport Settings");
    config.writeEntry("URL",  m_url);
    config.writeEntry("User", m_user);
    config.writeEntry("Gallery2", GalleryTalker::isGallery2());
    config.writeEntry("Resize", m_resizeCheckBox->isChecked());
    config.writeEntry("Maximum Width",  m_dimensionSpinBox->value());

    delete m_progressDlg;
    delete m_talker;
}

void GalleryWindow::slotHelp()
{
    KApplication::kApplication()->invokeHelp("galleryexport", "kipi-plugins");
}

void GalleryWindow::slotDoLogin()
{
    QString password;

#if KDE_IS_VERSION(3,2,0)
    if (!m_wallet)
        m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                               winId(),
                                               KWallet::Wallet::Synchronous );
    if (!m_wallet)
    {
        kdWarning() << "Failed to open kwallet" << endl;
    }
    else
    {
        if (!m_wallet->hasFolder("KIPIGalleryExportPlugin"))
        {
            if (!m_wallet->createFolder("KIPIGalleryExportPlugin"))
                kdWarning() << "Failed to create kwallet folder" << endl;
        }

        if (!m_wallet->setFolder("KIPIGalleryExportPlugin"))
            kdWarning() << "Failed to set kwallet folder" << endl;
        else
        {
            m_wallet->readPassword("password", password);
        }
    }
#endif


    GalleryLogin dlg( this, i18n( "Login Into Remote Gallery" ),
                      m_url, m_user, password, GalleryTalker::isGallery2() );
    if ( dlg.exec() != QDialog::Accepted )
    {
        close();
        return;
    }

    m_user           = dlg.name();
    GalleryTalker::setGallery2(dlg.isgGallery2Enable());

    KURL url(dlg.url());
    if (url.protocol().isEmpty())
    {
        url.setProtocol("http");
        url.setHost(dlg.url());
    }
    if (!url.url().endsWith(".php"))
    {
        if (GalleryTalker::isGallery2())
            url.addPath("main.php");
        else
            url.addPath("gallery_remote2.php");
    }
    m_url = url.url();

    QString newPassword = dlg.password();
#if KDE_IS_VERSION(3,2,0)
    if (newPassword != password && m_wallet)
        m_wallet->writePassword("password", newPassword);
#endif

    m_talker->login( url.url(), dlg.name(), newPassword );
}

void GalleryWindow::slotLoginFailed( const QString& msg )
{
    if ( KMessageBox::warningYesNo( this,
                                    i18n( "Failed to login into remote gallery. " )
                                    + msg
                                    + i18n("\nDo you want to try again?" ) )
         != KMessageBox::Yes )
    {
        close();
        return;
    }

    slotDoLogin();
}

void GalleryWindow::slotBusy( bool val )
{
    if ( val )
    {
        setCursor(QCursor::WaitCursor);
        m_newAlbumBtn->setEnabled( false );
        m_addPhotoBtn->setEnabled( false );
    }
    else
    {
        setCursor(QCursor::ArrowCursor);
        bool loggedIn = m_talker->loggedIn();
        m_newAlbumBtn->setEnabled( loggedIn );
        m_addPhotoBtn->setEnabled( loggedIn && m_albumView->selectedItem() );
    }
}

void GalleryWindow::slotError( const QString& msg )
{
    KMessageBox::error( this, msg );
}

void GalleryWindow::slotAlbums( const QValueList<GAlbum>& albumList )
{
    m_albumDict.clear();
    m_albumView->clear();
    m_photoView->begin();
    m_photoView->write( "<html></html>" );
    m_photoView->end();

    KIconLoader* iconLoader = KApplication::kApplication()->iconLoader();
    QPixmap pix = iconLoader->loadIcon( "folder", KIcon::NoGroup, 32 );

    typedef QValueList<GAlbum> GAlbumList;
    GAlbumList::const_iterator iter;
    for ( iter = albumList.begin(); iter != albumList.end(); ++iter )
    {
        const GAlbum& album = *iter;

        if ( album.parent_ref_num == 0 )
        {
            GAlbumViewItem* item = new GAlbumViewItem( m_albumView, album.title,
                                                       album );
            item->setPixmap( 0, pix );
            m_albumDict.insert( album.ref_num, item );
        }
        else
        {
            QListViewItem* parent = m_albumDict.find( album.parent_ref_num );
            if ( parent )
            {
                GAlbumViewItem* item = new GAlbumViewItem( parent, album.title,
                                                           album);
                item->setPixmap( 0, pix );
                m_albumDict.insert( album.ref_num, item );
            }
            else
            {
                kdWarning() << "Failed to find parent for album "
                            << album.name
                            << " with id " << album.ref_num << "\n";
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
            m_albumView->setSelected( lastSelectedItem, true );
            m_albumView->ensureItemVisible( lastSelectedItem );
        }
    }
}

void GalleryWindow::slotPhotos( const QValueList<GPhoto>& photoList)
{
    int pxSize = fontMetrics().height() - 2;
    QString styleSheet =
        QString( "body { margin: 8px; font-size: %1px; "
                 " color: %2; background-color: %3;}" )
        .arg( pxSize )
        .arg( colorGroup().text().name() )
        .arg( colorGroup().base().name() );

    styleSheet += QString( "a { font-size: %1px; color: %2; "
                           "text-decoration: none;}" )
                  .arg( pxSize )
                  .arg( colorGroup().text().name() );
    styleSheet += QString( "i { font-size: %1px; color: %2; "
                           "text-decoration: none;}" )
                  .arg( pxSize-2 )
                  .arg( QColor("steelblue").name() );

    m_photoView->begin();
    m_photoView->setUserStyleSheet( styleSheet );
    m_photoView->write( "<html>" );


    m_photoView->write("<table class='box-body' width='100%' "
                       "border='0' cellspacing='1' cellpadding='1'>" );


    typedef QValueList<GPhoto> GPhotoList;
    GPhotoList::const_iterator iter;
    for ( iter = photoList.begin(); iter != photoList.end(); ++iter )
    {
        const GPhoto& photo = *iter;
        KURL imageurl(photo.albumURL + photo.name);
        KURL thumburl(photo.albumURL + photo.thumbName);

        m_photoView->write( "<tr><td class='photo'>"
                            + QString("<a href='%1'>")
                            .arg(imageurl.url())
                            + QString("<img border=1 src=\"%1\"><br>")
                            .arg(thumburl.url())
                            + photo.name
                            + ( photo.caption.isEmpty() ? QString() :
                                QString("<br><i>%1</i>")
                                .arg(photo.caption) )
                            + "</a></td></tr>" );
    }

    m_photoView->write("</table>");

    m_photoView->write( "</html>" );
    m_photoView->end( );
}

void GalleryWindow::slotAlbumSelected()
{
    QListViewItem* item = m_albumView->selectedItem();
    if ( !item )
    {
        m_addPhotoBtn->setEnabled( false );
    }
    else
    {
        if ( m_talker->loggedIn() )
        {
            m_addPhotoBtn->setEnabled( true );

            m_photoView->begin();
            m_photoView->write( "<html></html>" );
            m_photoView->end();

            GAlbumViewItem* viewItem = static_cast<GAlbumViewItem*>(item);
            m_talker->listPhotos(viewItem->album.name);
            m_lastSelectedAlbum = viewItem->album.name;
        }
    }
}

void GalleryWindow::slotOpenPhoto( const KURL& url )
{
    new KRun(url);
}

void GalleryWindow::slotNewAlbum()
{
    GalleryAlbumDialog dlg;
    dlg.titleEdit->setFocus( );
    if ( dlg.exec() != QDialog::Accepted )
    {
        return;
    }

    QString name    = dlg.nameEdit->text();
    QString title   = dlg.titleEdit->text();
    QString caption = dlg.captionEdit->text();

    // check for prohibited chars in the album name
    // \ / * ? " ' & < > | . + # ( ) or spaces

    QChar ch;
    bool  clean = true;
    for (uint i=0; i<name.length(); i++)
    {
        ch = name[i];
        if (ch == '\\')
        {
            clean = false;
            break;
        }
        else if (ch == '/')
        {
            clean = false;
            break;
        }
        else if (ch == '*')
        {
            clean = false;
            break;
        }
        else if (ch == '?')
        {
            clean = false;
            break;
        }
        else if (ch == '"')
        {
            clean = false;
            break;
        }
        else if (ch == '\'')
        {
            clean = false;
            break;
        }
        else if (ch == '&')
        {
            clean = false;
            break;
        }
        else if (ch == '<')
        {
            clean = false;
            break;
        }
        else if (ch == '>')
        {
            clean = false;
            break;
        }
        else if (ch == '|')
        {
            clean = false;
            break;
        }
        else if (ch == '.')
        {
            clean = false;
            break;
        }
        else if (ch == '+')
        {
            clean = false;
            break;
        }
        else if (ch == '#')
        {
            clean = false;
            break;
        }
        else if (ch == '(')
        {
            clean = false;
            break;
        }
        else if (ch == ')')
        {
            clean = false;
            break;
        }
        else if (ch == ' ')
        {
            clean = false;
            break;
        }
    }

    if (!clean)
    {
        KMessageBox::error( this, i18n("Sorry, these characters are not allowed in album name: %1")
                            .arg("\\ / * ? \" \' & < > | . + # ( ) or spaces") );
        return;
    }

    QString parentAlbumName;

    QListViewItem* item = m_albumView->selectedItem();
    if (item)
    {
        GAlbumViewItem* viewItem = static_cast<GAlbumViewItem*>(item);
        parentAlbumName = viewItem->album.name;
    }
    else
    {
        parentAlbumName = "0";
    }

    m_talker->createAlbum(parentAlbumName, name, title, caption);
}

void GalleryWindow::slotAddPhotos()
{
    QListViewItem* item = m_albumView->selectedItem();
    if (!item)
        return;

    KURL::List urls = KIPI::ImageDialog::getImageURLs( this, m_interface );
    if (urls.isEmpty())
        return;

    typedef QPair<QString,QString> Pair;

    m_uploadQueue.clear();
    for (KURL::List::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        KIPI::ImageInfo info = m_interface->info( *it );
        m_uploadQueue.append( Pair( (*it).path(), info.description() ) );
    }

    m_uploadTotal = m_uploadQueue.count();
    m_uploadCount = 0;
    m_progressDlg->reset();
    slotAddPhotoNext();
}

void GalleryWindow::slotAddPhotoNext()
{
    if ( m_uploadQueue.isEmpty() )
    {
        m_progressDlg->reset();
        m_progressDlg->hide();
        slotAlbumSelected();
        return;
    }

    typedef QPair<QString,QString> Pair;
    Pair pathComments = m_uploadQueue.first();
    m_uploadQueue.pop_front();

    bool res = m_talker->addPhoto( m_lastSelectedAlbum, pathComments.first,
                                   pathComments.second,
                                   m_resizeCheckBox->isChecked(),
                                   m_dimensionSpinBox->value() );
    if (!res)
    {
        slotAddPhotoFailed( "" );
        return;
    }

    m_progressDlg->setLabelText( i18n("Uploading file %1 ")
                                 .arg( KURL(pathComments.first).filename() ) );

    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void GalleryWindow::slotAddPhotoSucceeded()
{
    m_uploadCount++;
    m_progressDlg->setProgress( m_uploadCount, m_uploadTotal );
    slotAddPhotoNext();
}

void GalleryWindow::slotAddPhotoFailed( const QString& msg )
{
    if ( KMessageBox::warningContinueCancel( this,
                                             i18n( "Failed to upload photo into "
                                                   "remote gallery. " )
                                             + msg
                                             + i18n("\nDo you want to continue?" ) )
         != KMessageBox::Continue )
    {
        m_uploadQueue.clear();
        m_progressDlg->reset();
        m_progressDlg->hide();

        // refresh the thumbnails
        slotAlbumSelected();
    }
    else
    {
        m_uploadTotal--;
        m_progressDlg->setProgress( m_uploadCount, m_uploadTotal );
        slotAddPhotoNext();
    }
}

void GalleryWindow::slotAddPhotoCancel()
{
    m_uploadQueue.clear();
    m_progressDlg->reset();
    m_progressDlg->hide();

    m_talker->cancel();

    // refresh the thumbnails
    slotAlbumSelected();
}

}

#include "gallerywindow.moc"

