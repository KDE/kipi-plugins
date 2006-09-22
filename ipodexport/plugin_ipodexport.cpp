/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

extern "C" {
#include <glib-object.h> //g_type_init
}

#include "ipodexportdialog.h"
#include "plugin_ipodexport.h"

#include <libkipi/imagecollection.h>


#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>


#define debug() kdDebug( 51000 )

typedef KGenericFactory<Plugin_iPodExport> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_ipodexport, Factory("kipiplugin_ipodexport"));

Plugin_iPodExport::Plugin_iPodExport( QObject *parent, const char*, const QStringList& )
    : KIPI::Plugin( Factory::instance(), parent, "iPodExport")
{
    kdDebug( 51001 ) << "Plugin_iPodExport plugin loaded" << endl;

    g_type_init();
}

void Plugin_iPodExport::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    // this is our action shown in the menubar/toolbar of the mainwindow
    m_actionImageUpload = new KAction( i18n( "Connect iPod..." ), "ipod", 0, this,
                                      SLOT( slotImageUpload() ), actionCollection(), "connectipod");

    addAction( m_actionImageUpload );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface )
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }
}


KIPI::Category Plugin_iPodExport::category( KAction* action ) const
{
    if ( action == m_actionBatchImageUpload )
       return KIPI::TOOLSPLUGIN;

    if ( action == m_actionImageUpload )
        return KIPI::IMAGESPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}


void Plugin_iPodExport::slotImageUpload()
{
    UploadDialog *dlg = new UploadDialog( m_interface, i18n("iPod Export"), kapp->activeWindow() );
    dlg->show();
}

