/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_expoblending.moc"

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

#include "manager.h"
#include "aboutdata.h"

using namespace KIPIExpoBlendingPlugin;

K_PLUGIN_FACTORY( ExpoBlendingFactory, registerPlugin<Plugin_ExpoBlending>(); )
K_EXPORT_PLUGIN ( ExpoBlendingFactory("kipiplugin_expoblending") )

Plugin_ExpoBlending::Plugin_ExpoBlending(QObject *parent, const QVariantList&)
                   : KIPI::Plugin(ExpoBlendingFactory::componentData(), parent, "ExpoBlending")
{
    m_interface    = 0;
    m_action       = 0;
    m_parentWidget = 0;
    m_manager      = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_ExpoBlending plugin loaded";
}

Plugin_ExpoBlending::~Plugin_ExpoBlending()
{
}

void Plugin_ExpoBlending::setup(QWidget* widget)
{
    m_parentWidget = widget;
    KIPI::Plugin::setup(m_parentWidget);

    m_action = actionCollection()->addAction("expoblending");
    m_action->setText(i18n("Blend bracketed images..."));
    m_action->setIcon(KIcon("expoblending"));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_action);

    m_interface = dynamic_cast< KIPI::Interface* >(parent());
    if (!m_interface)
    {
       kError() << "Kipi interface is null!";
       return;
    }
}

void Plugin_ExpoBlending::slotActivate()
{
    if (!m_interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KIPI::ImageCollection images = m_interface->currentSelection();

    if (!images.isValid() || images.images().isEmpty())
        return;

    if (!m_manager)
    {
        m_manager = new Manager(this);
        m_manager->setAbout(new ExpoBlendingAboutData());
    }

    if (!m_manager->checkBinaries())
        return;
    m_manager->setItemsList(images.images());
    m_manager->setIface(m_interface);
    m_manager->run();
}

KIPI::Category Plugin_ExpoBlending::category( KAction* action ) const
{
    if ( action == m_action )
       return KIPI::ToolsPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ToolsPlugin; // no warning from compiler, please
}
