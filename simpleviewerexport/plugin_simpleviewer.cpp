/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
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
#include <kapplication.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>

// Local includes.

#include "svedialog.h"
#include "simpleviewerexport.h"
#include "plugin_simpleviewer.h"
#include "plugin_simpleviewer.moc"

K_PLUGIN_FACTORY( SendImagesFactory, registerPlugin<Plugin_SimpleViewer>(); )
K_EXPORT_PLUGIN ( SendImagesFactory("kipiplugin_simpleviewer") )

Plugin_SimpleViewer::Plugin_SimpleViewer(QObject *parent, const QVariantList&)
                   : KIPI::Plugin(SendImagesFactory::componentData(), parent, "SimpleViewer")
{
    kDebug( 51001 ) << "Plugin_SimpleViewer plugin loaded" << endl;
}

void Plugin_SimpleViewer::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);
    
    m_actionSimpleViewer = new KAction(KIcon("applications-internet"), i18n("Flash Export..."), actionCollection());
    m_actionSimpleViewer->setObjectName("simpleviewer");
    connect(m_actionSimpleViewer, SIGNAL(triggered(bool)), 
            this, SLOT(slotActivate()));
    addAction(m_actionSimpleViewer);

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
    if ( !m_interface )
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }
}

KIPI::Category Plugin_SimpleViewer::category( KAction* action ) const
{
    if ( action == m_actionSimpleViewer )
       return KIPI::EXPORTPLUGIN;
    
    kWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::EXPORTPLUGIN; // no warning from compiler, please
}

void Plugin_SimpleViewer::slotActivate()
{
    if ( !m_interface )
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KIPISimpleViewerExportPlugin::SimpleViewerExport::run(m_interface, this);
}
