/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_flashexport.h"
#include "plugin_flashexport.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>

// Local includes

#include "svedialog.h"
#include "simpleviewer.h"

K_PLUGIN_FACTORY( FlashExportFactory, registerPlugin<Plugin_FlashExport>(); )
K_EXPORT_PLUGIN ( FlashExportFactory("kipiplugin_flashexport") )

Plugin_FlashExport::Plugin_FlashExport(QObject *parent, const QVariantList&)
                   : KIPI::Plugin(FlashExportFactory::componentData(), parent, "FlashExport")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_FlashExport plugin loaded" ;
}

void Plugin_FlashExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_flashexport");

    m_actionFlashExport = actionCollection()->addAction("flashexport");
    m_actionFlashExport->setText(i18n("Export to F&lash..."));
    m_actionFlashExport->setIcon(KIcon("flash"));
    m_actionFlashExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_L));

    connect(m_actionFlashExport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_actionFlashExport);

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
    if ( !m_interface )
    {
        kError() << "Kipi interface is null!" ;
        return;
    }
}

KIPI::Category Plugin_FlashExport::category( KAction* action ) const
{
    if ( action == m_actionFlashExport )
       return KIPI::ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification" ;
    return KIPI::ExportPlugin; // no warning from compiler, please
}

void Plugin_FlashExport::slotActivate()
{
    if ( !m_interface )
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KIPIFlashExportPlugin::SimpleViewer::run(m_interface, this);
}
