/* ============================================================
 * File  : plugin_commentseditor.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-09-26
 * Description :
 *
 * Copyright 2003 by Renchi Raju

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
 *
 * ============================================================ */

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kapplication.h>

// KIPI includes.

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes.

#include "plugin_commentseditor.h"
#include "commentseditor.h"

typedef KGenericFactory<Plugin_CommentsEditor> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_commentseditor,
                            Factory("kipiplugin_commentseditor"));

Plugin_CommentsEditor::Plugin_CommentsEditor(QObject *parent, const char*,
                                             const QStringList &)
                     : KIPI::Plugin( Factory::instance(), parent, "CommentsEditor")
{
    kdDebug( 51001 ) << "Plugin_CommentsEditor plugin loaded" << endl;
}


void Plugin_CommentsEditor::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_actionCommentsEditor = new KAction (i18n("Comments Editor..."),
                                          "editclear",
                                          0,
                                          this,
                                          SLOT(slotActivate()),
                                          actionCollection(),
                                          "commentseditor");

    addAction( m_actionCommentsEditor );

    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
    
    if ( !interface ) 
       {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
       }
    
    KIPI::ImageCollection images = interface->currentScope();
    m_actionCommentsEditor->setEnabled( images.isValid() );
    
    connect( interface, SIGNAL( currentScopeChanged( bool ) ), 
             m_actionCommentsEditor, SLOT( setEnabled( bool ) ) );
}

Plugin_CommentsEditor::~Plugin_CommentsEditor()
{
}

void Plugin_CommentsEditor::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
    
    if ( !interface ) 
       {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
       }

    KIPI::ImageCollection images = interface->currentScope();
    
    if ( !images.isValid() )
        return;

    if ( images.images().count() == 0 ) {
        KMessageBox::sorry(kapp->activeWindow(), i18n("Please select an album or a selection of images for editing comments !"));
        return;
    }

    KURL::List imgs = images.images();
    for( KURL::List::Iterator it = imgs.begin(); it != imgs.end(); ++it ) {
        qDebug(">%s", (*it).path().latin1() );
    }

    CommentsPlugin::CommentsEditor* editor =
        new CommentsPlugin::CommentsEditor( interface, images, kapp->activeWindow() );
    editor->show();

}

void Plugin_CommentsEditor::slotAlbumChanged(Digikam::AlbumInfo* album)
{
    if (!album)
        m_actionCommentsEditor->setEnabled(false);
    else
        m_actionCommentsEditor->setEnabled(true);
}

KIPI::Category Plugin_CommentsEditor::category( KAction* action ) const
{
    if ( action == m_actionCommentsEditor )
       return KIPI::BATCHPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;       
    return KIPI::BATCHPLUGIN; // no warning from compiler, please           
}

#include "plugin_commentseditor.moc"
