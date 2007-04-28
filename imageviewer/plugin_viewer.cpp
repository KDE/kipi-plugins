/***************************************************************************
 *   Copyright (C) 2007 by Markus Leuthold   *
 *   <kusi at forum.titlis.org>   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.        *
 ***************************************************************************/
#include "plugin_viewer.h"
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <qmessagebox.h>
#include <libkipi/imageinfo.h>
#include <iostream>
#include "ogl.h"

typedef KGenericFactory<Plugin_viewer> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_viewer,
                            Factory("kipiplugin_viewer"))

Plugin_viewer::Plugin_viewer( QObject *parent, const char* name, const QStringList& )
    :KIPI::Plugin::Plugin( Factory::instance(), parent, name )
{
	kdDebug(51001) << "image viewer plugin loaded" << endl;
}

void Plugin_viewer::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
	
	if ( !interface ) 
    {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
    }

	actionViewer = new KAction (i18n("Image Viewer"),
                                     "ogl",
                                     0, // do never set shortcuts from plugins.
                                     this,
                                     SLOT(slotActivate()),
                                     actionCollection(),
                                     "viewer");
	addAction(actionViewer);
	widget=0;
//     m_dialog = new ExampleDialog( interface, 0, "example plugin dialog" );
//     m_dialog->resize( 800, 600 );
// 
//     m_imageInfo = new KAction( i18n( "Show Image Info" ), "", 0, this, SLOT( action() ), actionCollection(), "showImageInfo" );
//     addAction( m_imageInfo );
// 
//     connect( interface, SIGNAL( currentAlbumChanged( bool ) ),  m_dialog, SLOT( reload() ) );
}

KIPI::Category Plugin_viewer::category( KAction* action ) const
{
    if ( action == actionViewer ) {
        return KIPI::TOOLSPLUGIN;
    }
    else {
        kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
        return KIPI::TOOLSPLUGIN; // no warning from compiler, please
    }
}





/*!
    \fn Plugin_viewer::slotActivate()
 */
void  Plugin_viewer::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
	
	if ( !interface ) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }
	
	widget = new KIPIviewer::ogl(interface);
	
	switch(widget->getOGLstate()) {
		case KIPIviewer::oglOK:
			widget->show();
			break;
			
		case KIPIviewer::oglNoRectangularTexture:
			kdError( 51000 ) << "GL_ARB_texture_rectangle not supported" << endl;
			delete widget;
			QMessageBox::critical(new QWidget(),"OpenGL error","GL_ARB_texture_rectangle not supported");
			break;
			
		case KIPIviewer::oglNoContext:
			kdError( 51000 ) << "no OpenGL context found" << endl;
			delete widget;
			QMessageBox::critical(new QWidget(),"OpenGL error","no OpenGL context found");
	}
}
