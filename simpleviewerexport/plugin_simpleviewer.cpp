/* ============================================================
 * File  : plugin_simpleviewer.cpp
 * Author: Joern Ahrens <joern.ahrens@kdemail.net>
 * Date  : 2005-12-19
 * Description : KIPI Exportplugin to SimpleViewer
 *
 * Copyright 2005 by Joern Ahrens
 *
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
 * ============================================================ */

// KDE includes.
  
#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>

// Local includes.

#include "plugin_simpleviewer.h"
#include "simpleviewerexport.h"

// A macro from KDE KParts to export the symbols for this plugin and
// create the factory for it. The first argument is the name of the
// plugin library and the second is the genericfactory templated from
// the class for your plugin
typedef KGenericFactory<Plugin_SimpleViewer> Factory;
K_EXPORT_COMPONENT_FACTORY( kipiplugin_simpleviewer, Factory("kipiplugin_simpleviewer"));

Plugin_SimpleViewer::Plugin_SimpleViewer(QObject *parent, const char*, const QStringList&)
    : KIPI::Plugin(Factory::instance(), parent, "SimpleViewer")
{
    kdDebug( 51001 ) << "Plugin_SimpleViewer plugin loaded" << endl;
}

void Plugin_SimpleViewer::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    
    // action shown in the menubar/toolbar of the mainwindow
    m_actionSimpleViewer = new KAction (i18n("Export to SimpleViewer ..."),    // menu message
                                   "www",
                                   0,
                                   this,
                                   SLOT(slotActivate()),
                                   actionCollection(),
                                   "simpleviewer");
    
    addAction( m_actionSimpleViewer );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
    if ( !m_interface )
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }
}

KIPI::Category Plugin_SimpleViewer::category( KAction* action ) const
{
    if ( action == m_actionSimpleViewer )
       return KIPI::EXPORTPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::EXPORTPLUGIN; // no warning from compiler, please
}

void Plugin_SimpleViewer::slotActivate()
{
    if ( !m_interface )
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }
    
    KIPISimpleViewerExportPlugin::SimpleViewerExport::run( m_interface, this );
}

#include "plugin_simpleviewer.moc"

