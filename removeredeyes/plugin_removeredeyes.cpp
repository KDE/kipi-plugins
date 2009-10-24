/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : a kipi plugin to automatically detect
 *               and remove red eyes from images
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "plugin_removeredeyes.h"
#include "plugin_removeredeyes.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes

#include "removeredeyeswindow.h"

K_PLUGIN_FACTORY( RemoveRedEyesFactory, registerPlugin<Plugin_RemoveRedEyes>(); )
K_EXPORT_PLUGIN ( RemoveRedEyesFactory("kipiplugin_removeredeyes") )

Plugin_RemoveRedEyes::Plugin_RemoveRedEyes(QObject *parent, const QVariantList &/*args*/)
                    : KIPI::Plugin(RemoveRedEyesFactory::componentData(),
                                   parent, "RemoveRedEyes")
{

    kDebug(AREA_CODE_LOADING) << "Plugin_RemoveRedEyes plugin loaded";
}

void Plugin_RemoveRedEyes::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_action = actionCollection()->addAction("removeredeyes");
    m_action->setText(i18n("&Red-Eye Removal..."));
    m_action->setIcon(KIcon("draw-eraser"));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(activate()));

    addAction(m_action);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*> (parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    connect(interface, SIGNAL(selectionChanged(bool)),
            m_action, SLOT(setEnabled(bool)));
}

Plugin_RemoveRedEyes::~Plugin_RemoveRedEyes()
{
}

void Plugin_RemoveRedEyes::activate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KIPIRemoveRedEyesPlugin::RemoveRedEyesWindow* window =
        new KIPIRemoveRedEyesPlugin::RemoveRedEyesWindow(interface);
    window->show();
}

KIPI::Category Plugin_RemoveRedEyes::category(KAction* action) const
{
    if (action == m_action)
        return KIPI::BatchPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::BatchPlugin; // no warning from compiler, please
}
