//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_IMAGESGALLERY.CPP
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Include files for KDE

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

// Local include files

#include "plugin_imagesgallery.h"
#include "imagesgallery.h"

typedef KGenericFactory<Plugin_Imagesgallery> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_imagesgallery,
                            Factory("kipiplugin_imagesgallery"));

// -----------------------------------------------------------
Plugin_Imagesgallery::Plugin_Imagesgallery(QObject *parent, const char*, const QStringList&)
                    : KIPI::Plugin( Factory::instance(), parent, "ImagesGallery")
{
    kdDebug( 51001 ) << "Plugin_Imagesgallery plugin loaded" << endl;
}

void Plugin_Imagesgallery::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    m_actionImagesGallery = new KAction (i18n("HTML export..."),        // Menu message.
                                         "www",                         // Menu icon.
                                         0,
                                         this,
                                         SLOT(slotActivate()),
                                         actionCollection(),
                                        "images_gallery");

    addAction( m_actionImagesGallery );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_Imagesgallery::~Plugin_Imagesgallery()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_Imagesgallery::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
   
    if ( !interface ) 
       {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
       }

    KIPIImagesGalleryPlugin::ImagesGallery Gallery( interface );
}


KIPI::Category Plugin_Imagesgallery::category( KAction* action ) const
{
    if ( action == m_actionImagesGallery )
       return KIPI::EXPORTPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::EXPORTPLUGIN; // no warning from compiler, please
}

#include "plugin_imagesgallery.moc"
