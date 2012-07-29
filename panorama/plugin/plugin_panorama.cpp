/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_panorama.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes

#include "manager/manager.h"
#include "aboutdata.h"

namespace KIPIPanoramaPlugin
{

K_PLUGIN_FACTORY( PanoramaFactory, registerPlugin<Plugin_Panorama>(); )
K_EXPORT_PLUGIN ( PanoramaFactory("kipiplugin_panorama") )

Plugin_Panorama::Plugin_Panorama(QObject* const parent, const QVariantList&)
    : Plugin(PanoramaFactory::componentData(), parent, "Panorama")
{
    m_interface    = 0;
    m_action       = 0;
    m_parentWidget = 0;
    m_manager      = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_Panorama plugin loaded";
}

Plugin_Panorama::~Plugin_Panorama()
{
}

void Plugin_Panorama::setup(QWidget* widget)
{
    m_parentWidget = widget;
    Plugin::setup(m_parentWidget);

    m_action = actionCollection()->addAction("panorama");
    m_action->setText(i18n("Stitch images into a panorama..."));
    m_action->setIcon(KIcon("layer-visible-on"));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_action);

    m_interface = dynamic_cast< Interface* >(parent());
    if (!m_interface)
    {
       kError() << "Kipi interface is null!";
       return;
    }
}

void Plugin_Panorama::slotActivate()
{
    if (!m_interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    ImageCollection images = m_interface->currentSelection();

    if (!images.isValid() || images.images().isEmpty())
        return;

    if (m_manager)
    {
        delete m_manager;
    }

    m_manager = new Manager(this);
    m_manager->checkBinaries();
    m_manager->setItemsList(images.images());
    m_manager->setIface(m_interface);
    m_manager->run();
}

Category Plugin_Panorama::category( KAction* action ) const
{
    if ( action == m_action )
       return ToolsPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ToolsPlugin; // no warning from compiler, please
}

} // namespace KIPIPanoramaPlugin
