/* ============================================================
 * File  : gallerywindow.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Description :
 *
 * Copyright 2004 by Renchi Raju

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

#include <qlistview.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qlineedit.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <krun.h>
#include <kdebug.h>

#include "gallerytalker.h"
#include "galleryitem.h"
#include "galleryviewitem.h"
#include "gallerylogin.h"
#include "gallerywidget.h"
#include "galleryalbumdialog.h"
#include "gallerywindow.h"

GalleryWindow::GalleryWindow()
    : KDialogBase(0, 0, true, i18n( "Gallery Export" ), Close, Close, false)
{
    GalleryWidget* widget = new GalleryWidget( this );
    setMainWidget( widget );
    widget->setMinimumSize( 600, 400 );

    m_albumView   = widget->m_albumView;
    m_photoView   = widget->m_photoView;
    m_newAlbumBtn = widget->m_newAlbumBtn;
    m_addPhotoBtn = widget->m_addPhotoBtn;

    m_albumView->setRootIsDecorated( true );

    m_newAlbumBtn->setEnabled( false );
    m_addPhotoBtn->setEnabled( false );

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
    
    QTimer::singleShot( 0, this,  SLOT( slotDoLogin() ) );
}

GalleryWindow::~GalleryWindow()
{
    delete m_talker;
}

void GalleryWindow::slotDoLogin()
{
    GalleryLogin dlg( this, i18n( "Login into remote gallery" ),
                      m_url, m_user );
    if ( dlg.exec() != QDialog::Accepted )
    {
        close();
        return;
    }

    KURL url(dlg.url());
    if (url.protocol().isEmpty())
    {
        url.setProtocol("http");
        url.setHost(dlg.url());
    }
    if (!url.url().endsWith(".php"))
        url.addPath("gallery_remote2.php");

    m_url  = url.url();
    m_user = dlg.name();
    
    m_talker->login( url.url(), dlg.name(), dlg.password() );
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
            GAlbumViewItem* item = new GAlbumViewItem( m_albumView, album.name,
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
        KURL imageurl(photo.albumURL);
        KURL thumburl(photo.albumURL);
        imageurl.addPath(photo.name);
        thumburl.addPath(photo.thumbName);

        m_photoView->write( "<tr><td class='photo'>"
                            + QString("<a href='%1'>")
                            .arg(imageurl.url())
                            + QString("<img border=1 src=%1><br>")
                            .arg(thumburl.url())
                            + photo.name
                            + "</a></td></tr>");
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
    dlg.nameEdit->setFocus( );
    if ( dlg.exec() != QDialog::Accepted )
    {
        return;
    }

    QString name    = dlg.nameEdit->text();
    QString title   = dlg.titleEdit->text();
    QString caption = dlg.captionEdit->text();

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
    
}

#include "gallerywindow.moc"
