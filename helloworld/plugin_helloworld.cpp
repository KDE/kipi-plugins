/* ============================================================
 * File  : plugin_helloworld.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-03-10
 * Description :
 *
 * Copyright 2003 by Renchi Raju
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * ============================================================ */

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

#include "plugin_helloworld.h"
#include <libkipi/imagecollection.h>

// A macro from KDE KParts to export the symbols for this plugin and
// create the factory for it. The first argument is the name of the
// plugin library and the second is the genericfactory templated from
// the class for your plugin
typedef KGenericFactory<Plugin_HelloWorld> Factory;
K_EXPORT_COMPONENT_FACTORY( kipiplugin_helloworld,
                            Factory("kipiplugin_helloworld"));

Plugin_HelloWorld::Plugin_HelloWorld(QObject *parent,
                                     const char*,
                                     const QStringList&)
    : KIPI::Plugin( Factory::instance(), parent, "HelloWorld")
{
    kdDebug( 51001 ) << "Plugin_HelloWorld plugin loaded" << endl;
}

void Plugin_HelloWorld::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    // this is our action shown in the menubar/toolbar of the mainwindow
    KAction* action = new KAction (i18n("Hello World..."),
                                   "misc",
                                   0,	// do never set shortcuts from plugins.
                                   this,
                                   SLOT(slotActivate()),
                                   actionCollection(),
                                   "helloworld");
    addAction( action );

    m_interface = static_cast< KIPI::Interface* >( parent() );
}

void Plugin_HelloWorld::slotActivate()
{
    kdDebug( 51000 ) << "Plugin_HelloWorld slot activated" << endl;

    // Print some information about the capabilities of the host application.
    kdDebug( 51000 ) << "Features supported by the host application:" << endl;
    kdDebug( 51000 ) << "  AlbumsHaveComments:  " << (m_interface->hasFeature( KIPI::AlbumsHaveComments ) ? "Yes" : "No") << endl;
    kdDebug( 51000 ) << "  AlbumEQDir:          " << (m_interface->hasFeature( KIPI::AlbumEQDir ) ? "Yes" : "No") << endl;
    kdDebug( 51000 ) << "  ImagesHasComments:   " << (m_interface->hasFeature( KIPI::ImagesHasComments ) ? "Yes" : "No") << endl;
    kdDebug( 51000 ) << "  ImagesHasTime:       " << (m_interface->hasFeature( KIPI::ImagesHasTime ) ? "Yes" : "No") << endl;
    kdDebug( 51000 ) << "  SupportsDateRanges:  " << (m_interface->hasFeature( KIPI::SupportsDateRanges ) ? "Yes" : "No") << endl;
    kdDebug( 51000 ) << "  AcceptNewImages:     " << (m_interface->hasFeature( KIPI::AcceptNewImages ) ? "Yes" : "No") << endl;
    kdDebug( 51000 ) << "  ImageTitlesWritable: " << (m_interface->hasFeature( KIPI::ImageTitlesWritable ) ? "Yes" : "No") << endl;


    // ================================================== Selection
    kdDebug( 51000 ) << endl
                     << "==================================================" << endl
                     << "                    Selection                     " << endl
                     << "==================================================" << endl;
    KIPI::ImageCollection selection = m_interface->currentSelection();
    if ( !selection.isValid() ) {
        kdDebug( 51000) << "No Selection!" << endl;
    }
    else {
        KURL::List images = selection.images();
        for( KURL::List::Iterator selIt = images.begin(); selIt != images.end(); ++selIt ) {
            kdDebug( 51000 ) <<  *selIt << endl;
            KIPI::ImageInfo info = m_interface->info( *selIt );
            kdDebug( 51000 ) << "\ttitle: " << info.title() << endl;
            if ( m_interface->hasFeature( KIPI::ImagesHasComments ) )
                kdDebug( 51000 ) << "\tdescription: " << info.description() << endl;
        }
    }

    // ================================================== Current Album
    kdDebug( 51000 ) << endl
                     << "==================================================" << endl
                     << "                    Current Album                 " << endl
                     << "==================================================" << endl;
    KIPI::ImageCollection album = m_interface->currentAlbum();
    if ( !album.isValid() ) {
        kdDebug( 51000 ) << "No album!" << endl;
    }
    else {
        KURL::List images = album.images();
        for( KURL::List::Iterator albumIt = images.begin(); albumIt != images.end(); ++albumIt ) {
            kdDebug( 51000 ) <<  *albumIt << endl;
        }

        kdDebug( 51000 ) << "Album name: " << album.name() << endl;
        if ( m_interface->hasFeature( KIPI::AlbumsHaveComments ) ) {
            kdDebug( 51000 ) << "Album Comment: " << album.comment() << endl;
        }
    }
}

KIPI::Category Plugin_HelloWorld::category() const
{
    return KIPI::IMAGESPLUGIN;
}

