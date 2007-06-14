/* ============================================================
 * File  : plugin_slideshow.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-31
 * Description :
 *
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>

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

 // KDE includes.
 
#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <qvaluelist.h>
#include <qpair.h>
#include <qstringlist.h>

#include <cstdlib>
#include <sys/time.h>
#include <time.h>

// Local includes.

#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowconfig.h"
#include "plugin_slideshow.h"

// Lib KIPI includes.
 
#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

typedef KGenericFactory<Plugin_SlideShow> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_slideshow,
                            Factory("kipiplugin_slideshow"));

Plugin_SlideShow::Plugin_SlideShow(QObject *parent,
                                   const char*,
                                   const QStringList&)
    : KIPI::Plugin( Factory::instance(), parent, "SlideShow")
{
    kdDebug( 51001 ) << "Plugin_SlideShow plugin loaded"
                     << endl;
}

void Plugin_SlideShow::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    
    m_actionSlideShow = new KAction (i18n("Advanced SlideShow..."),
                                     "slideshow",
                                     0,
                                     this,
                                     SLOT(slotActivate()),
                                     actionCollection(),
                                     "slideshow");

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
    
    m_urlList = new KURL::List();

    if ( !m_interface ) 
    {
       kdError( 51000 ) << "Kipi m_interface is null!" << endl;
       return;
    }

    m_actionSlideShow->setEnabled( false );
    
    connect( m_interface, SIGNAL( currentAlbumChanged( bool ) ),
             SLOT( slotAlbumChanged( bool ) ) );

    addAction( m_actionSlideShow );
}


Plugin_SlideShow::~Plugin_SlideShow()
{
    if (m_urlList)
        delete m_urlList;
}


void Plugin_SlideShow::slotActivate()
{

    if ( !m_interface ) 
    {
        kdError( 51000 ) << "Kipi m_interface is null!" << endl;
        return;
    }

    bool allowSelectedOnly = true;

    KIPI::ImageCollection currSel = m_interface->currentSelection();
    if ( !currSel.isValid() || currSel.images().isEmpty() )
    {
        allowSelectedOnly = false;
    }

    m_imagesHasComments = m_interface->hasFeature(KIPI::ImagesHasComments);
    
    KIPISlideShowPlugin::SlideShowConfig *slideShowConfig
            = new KIPISlideShowPlugin::SlideShowConfig( allowSelectedOnly, m_interface,kapp->activeWindow(), 
                                                        i18n("Slide Show").ascii(), m_imagesHasComments,
                                                        m_urlList);

    connect(slideShowConfig, SIGNAL(buttonStartClicked()),
             this, SLOT(slotSlideShow()));

     slideShowConfig->show();
}

void Plugin_SlideShow::slotAlbumChanged(bool anyAlbum)
{
    if (!anyAlbum)
    {
        m_actionSlideShow->setEnabled( false );
        return;
    }

    KIPI::Interface* m_interface = dynamic_cast<KIPI::Interface*>( parent() );
    if ( !m_interface ) 
    {
        kdError( 51000 ) << "Kipi m_interface is null!" << endl;
        m_actionSlideShow->setEnabled( false );
        return;
    }

    KIPI::ImageCollection currAlbum = m_interface->currentAlbum();
    if ( !currAlbum.isValid() )
    {
        kdError( 51000 ) << "Current image collection is not valid." << endl;
        m_actionSlideShow->setEnabled( false );
        return;
    }

    m_actionSlideShow->setEnabled(true);
}

void Plugin_SlideShow::slotSlideShow()
{

    if ( !m_interface ) 
    {
           kdError( 51000 ) << "Kipi m_interface is null!" << endl;
           return;
    }

    KConfig config("kipirc");
     
    bool    opengl;
    bool    shuffle;
    
    config.setGroup("SlideShow Settings");
    opengl                = config.readBoolEntry("OpenGL");
    shuffle               = config.readBoolEntry("Shuffle");

    if ( !m_urlList ) {kdDebug() << "ERRORE m_urlList NON ALLOCATO!!!"; return;}

    if ( m_urlList->isEmpty() )
    {
        KMessageBox::sorry(kapp->activeWindow(), i18n("There are no images to show."));
	return;
    }

    typedef QPair<QString, int> FileAnglePair;
    typedef QValueList<FileAnglePair > FileList;
    FileList fileList;
    QStringList commentsList;

    for( KURL::List::Iterator urlIt = m_urlList->begin(); urlIt != m_urlList->end(); ++urlIt )
    {
        KIPI::ImageInfo info = m_interface->info( *urlIt );
        fileList.append( FileAnglePair((*urlIt).path(), info.angle()) );
        commentsList.append(info.description());
    }

    m_urlList->clear();
    
    if (shuffle)
    {
        struct timeval tv;
        gettimeofday(&tv, 0);
        srand(tv.tv_sec);

        FileList::iterator it  = fileList.begin();
        FileList::iterator it1;

        QStringList::iterator itcom = commentsList.begin();
        QStringList::iterator itcom1;

        for (uint i=0; i<fileList.size(); i++)
        {
            int inc = (int) (float(fileList.count())*rand()/(RAND_MAX+1.0));

            it1 = fileList.begin();
            it1 += inc;

            itcom1 = commentsList.begin();
            itcom1 += inc;

            qSwap(*(it++), *(it1));
            qSwap(*(itcom++), *(itcom1));
        }
    }


    if (!opengl) {
        KIPISlideShowPlugin::SlideShow *slideShow =
                new KIPISlideShowPlugin::SlideShow(fileList, commentsList, m_imagesHasComments);
        slideShow->show();
    }
    else {
        if (!QGLFormat::hasOpenGL())
            KMessageBox::error(kapp->activeWindow(),
                               i18n("Sorry. OpenGL support not available on your system"));
        else {
            KIPISlideShowPlugin::SlideShowGL *slideShow =
                    new KIPISlideShowPlugin::SlideShowGL(fileList, commentsList, m_imagesHasComments);
            slideShow->show();
        }
    }
}

KIPI::Category Plugin_SlideShow::category( KAction* action ) const
{
    if ( action == m_actionSlideShow )
       return KIPI::TOOLSPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::TOOLSPLUGIN; // no warning from compiler, please
}


#include "plugin_slideshow.moc"
