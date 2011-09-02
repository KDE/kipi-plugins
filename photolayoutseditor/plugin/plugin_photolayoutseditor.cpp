/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_photolayoutseditor.h"

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kicon.h>
#include <kaction.h>
#include <kactioncollection.h>

#include <libkipi/interface.h>

#include "photolayoutseditor.h"

using namespace KIPIPhotoLayoutsEditor;

K_PLUGIN_FACTORY ( PhotoFrmesEditorFactory, registerPlugin<Plugin_PhotoLayoutsEditor>(); )
K_EXPORT_PLUGIN  ( PhotoFrmesEditorFactory("kipiplugin_photolayoutseditor") )

Plugin_PhotoLayoutsEditor::Plugin_PhotoLayoutsEditor(QObject * parent, const QVariantList & args) :
    KIPI::Plugin( PhotoFrmesEditorFactory::componentData(), parent, "Photo Frames Editor" )
{
    m_interface    = 0;
    m_action       = 0;
    m_parentWidget = 0;
    m_manager      = 0;

    kDebug() << "Plugin_PhotoLayoutsEditor plugin loaded";
}

Plugin_PhotoLayoutsEditor::~Plugin_PhotoLayoutsEditor()
{}

KIPI::Category Plugin_PhotoLayoutsEditor::category( KAction * action ) const
{
    if ( action == m_action )
       return KIPI::ToolsPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ToolsPlugin; // no warning from compiler, please
}

void Plugin_PhotoLayoutsEditor::setup( QWidget * widget )
{
    m_parentWidget = widget;
    KIPI::Plugin::setup(m_parentWidget);

    m_action = actionCollection()->addAction("photolayoutseditor");
    m_action->setText(i18n("Create photo layouts..."));
    m_action->setIcon(KIcon("photolayoutseditor"));

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

void Plugin_PhotoLayoutsEditor::slotActivate()
{
    if (!m_interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_manager = PhotoLayoutsEditor::instance(m_parentWidget);
    m_manager->setInterface(m_interface);
    m_manager->show();
}
