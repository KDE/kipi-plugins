/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-05-16
 * Description : A plugin to create KML files to present images with coordinates.
 *
 * Copyright (C) 2006-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_kmlexport.h"

// Qt includes

#include <QApplication>
#include <QPointer>
#include <QAction>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>

// Libkipi includes

#include <KIPI/ImageCollection>
#include <KIPI/Interface>

// Local includes

#include "kmlexport.h"
#include "kmlwindow.h"
#include "kipiplugins_debug.h"

namespace KIPIKMLExportPlugin
{

K_PLUGIN_FACTORY( KMLExportFactory, registerPlugin<Plugin_KMLExport>(); )

Plugin_KMLExport::Plugin_KMLExport(QObject* const parent, const QVariantList&)
    : Plugin( /*KMLExportFactory::componentData(),*/ parent, "KMLExport")
{
    m_actionKMLExport = 0;
    m_interface       = 0;

    qCDebug(KIPIPLUGINS_LOG) << "Plugin_SendImages plugin loaded";

    setUiBaseName("kipiplugin_kmlexportui.rc");
    setupXML();
}

Plugin_KMLExport::~Plugin_KMLExport()
{
}

void Plugin_KMLExport::setup(QWidget* const widget)
{
    Plugin::setup( widget );

    setupActions();

    m_interface = interface();

    if (!m_interface)
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!" ;
        return;
    }

    m_actionKMLExport->setEnabled(true);
}

void Plugin_KMLExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionKMLExport = new QAction(this);
    m_actionKMLExport->setText(i18n("Export to KML..."));
    m_actionKMLExport->setIcon(QIcon::fromTheme(QLatin1String("applications-development-web")));
    m_actionKMLExport->setEnabled(false);

    connect(m_actionKMLExport, SIGNAL(triggered(bool)),
            this, SLOT(slotKMLExport()));

    addAction(QLatin1String("kmlexport"), m_actionKMLExport);
}

void Plugin_KMLExport::slotKMLExport()
{
    if (!m_interface)
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!" ;
        return;
    }

    ImageCollection selection = m_interface->currentSelection();

    if (!selection.isValid())
    {
        qCDebug(KIPIPLUGINS_LOG) << "No Selection!";
        return;
    }

    KmlWindow* const dlg = new KmlWindow(
        QApplication::activeWindow(),
        m_interface->hasFeature(ImagesHasComments),
        m_interface->hasFeature(ImagesHasTime),
        m_interface->currentAlbum().name(),
        m_interface->currentSelection());
    dlg->show();
}

} // namespace KIPIKMLExportPlugin

#include "plugin_kmlexport.moc"
