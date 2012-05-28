/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-05-16
 * Description : A plugin to create KML files to present images with coordinates.
 *
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
 *
 * ============================================================ */

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_kmlexport.moc"

// Qt includes

#include <QPointer>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes

#include "kmlexport.h"
#include "kmlexportconfig.h"

namespace KIPIKMLExportPlugin
{

K_PLUGIN_FACTORY( KMLExportFactory, registerPlugin<Plugin_KMLExport>(); )
K_EXPORT_PLUGIN ( KMLExportFactory("kipiplugin_kmlexport") )

Plugin_KMLExport::Plugin_KMLExport(QObject* const parent, const QVariantList&)
    : Plugin( KMLExportFactory::componentData(), parent, "KMLExport")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_KMLExport plugin loaded" ;
}

Plugin_KMLExport::~Plugin_KMLExport()
{
}

void Plugin_KMLExport::setup(QWidget* const widget)
{
    Plugin::setup( widget );

    m_actionKMLExport = actionCollection()->addAction("kmlexport");
    m_actionKMLExport->setText(i18n("Export to KML..."));
    m_actionKMLExport->setIcon(KIcon("applications-development-web"));

    connect(m_actionKMLExport, SIGNAL(triggered(bool)),
            this, SLOT(slotKMLExport()));

    addAction( m_actionKMLExport );

    m_interface = dynamic_cast<Interface*>( parent() );

    if ( !m_interface )
    {
        kError() << "Kipi interface is null!" ;
        return;
    }
}

void Plugin_KMLExport::slotKMLExport()
{
    ImageCollection selection = m_interface->currentSelection();

    if ( !selection.isValid() )
    {
        kDebug() << "No Selection!" ;
    }
    else
    {
        KMLExportConfig* kmlExportConfigGui = new KMLExportConfig(kapp->activeWindow());

        connect(kmlExportConfigGui, SIGNAL(okButtonClicked()),
                this, SLOT(slotKMLGenerate()));

        kmlExportConfigGui->show();
    }
}

void Plugin_KMLExport::slotKMLGenerate()
{
    ImageCollection selection = m_interface->currentSelection();
    KmlExport myExport(m_interface);
    if(!myExport.getConfig())
        return;

    myExport.generate();
}

Category Plugin_KMLExport::category(KAction* const action) const
{
    if ( action == m_actionKMLExport )
    {
       return ExportPlugin;
    }

    kWarning() << "Unrecognized action for plugin category identification" ;
    return ExportPlugin; // no warning from compiler, please
}

} // namespace KIPIKMLExportPlugin
