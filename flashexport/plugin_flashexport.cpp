/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-20
 * Description : a tool to export images to flash
 *
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#include "plugin_flashexport.moc"

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

#include "flashmanager.h"
#include "aboutdata.h"

namespace KIPIFlashExportPlugin
{

K_PLUGIN_FACTORY( FlashExportFactory, registerPlugin<Plugin_FlashExport>(); )
K_EXPORT_PLUGIN ( FlashExportFactory("kipiplugin_flashexport") )

Plugin_FlashExport::Plugin_FlashExport(QObject* const parent, const QVariantList&)
    : Plugin(FlashExportFactory::componentData(), parent, "FlashExport")
{
    m_interface    = 0;
    m_action       = 0;
    m_parentWidget = 0;
    m_manager      = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_Flashexport plugin loaded";
}

Plugin_FlashExport::~Plugin_FlashExport()
{
}

void Plugin_FlashExport::setup(QWidget* widget)
{
    m_parentWidget = widget;
    Plugin::setup(m_parentWidget);

    m_action = actionCollection()->addAction("flashexport");
    m_action->setText(i18n("Export to F&lash..."));
    m_action->setIcon(KIcon("flash"));
    m_action->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_L));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_action);

    m_interface = dynamic_cast<Interface*>(parent());
    if (!m_interface)
    {
       kError() << "Kipi interface is null!";
       return;
    }
}

void Plugin_FlashExport::slotActivate()
{
    if (!m_interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    if (!m_manager)
    {
        m_manager = new FlashManager(this);
    }

    m_manager->setIface(m_interface);
    m_manager->run();
}

Category Plugin_FlashExport::category(KAction* action) const
{
    if ( action == m_action )
       return ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification" ;
    return ExportPlugin; // no warning from compiler, please
}

} // namespace KIPIFlashExportPlugin