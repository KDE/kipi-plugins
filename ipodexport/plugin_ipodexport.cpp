/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <me at sebruiz dot net>
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

extern "C" 
{
#include <glib-object.h> //g_type_init
}

#include "ipodexportdialog.h"
#include "plugin_ipodexport.h"

#include <KActionCollection>
#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>

#include <libkipi/imagecollection.h>

using namespace KIPIIpodExportPlugin;

K_PLUGIN_FACTORY( IpodFactory, registerPlugin<Plugin_iPodExport>(); )
K_EXPORT_PLUGIN ( IpodFactory("kipiplugin_ipodexport") )

Plugin_iPodExport::Plugin_iPodExport( QObject *parent, const QVariantList& )
                 : KIPI::Plugin( IpodFactory::componentData(), parent, "iPodExport")
{
    kDebug( 51001 ) << "Plugin_iPodExport plugin loaded" << endl;

    g_type_init();
}

void Plugin_iPodExport::setup( QWidget* widget )
{
    KIPI::Plugin::setup(widget);

    m_actionImageUpload = new KAction(i18n( "Export to iPod..." ), actionCollection());

    m_actionImageUpload->setIcon(KIcon("multimedia-player-apple-ipod"));
    connect(m_actionImageUpload, SIGNAL(triggered(bool)),
            this, SLOT(slotImageUpload()));
    addAction( m_actionImageUpload );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
}

void Plugin_iPodExport::slotImageUpload()
{
    UploadDialog *dlg = new UploadDialog(m_interface, i18n("iPod Export"),
                                         kapp->activeWindow());
    dlg->setMinimumWidth(460);
    dlg->show();
}

KIPI::Category Plugin_iPodExport::category( KAction* action ) const
{
    if ( action == m_actionImageUpload )
        return KIPI::ExportPlugin;

    return KIPI::ImagesPlugin; // no warning from compiler, please
}
