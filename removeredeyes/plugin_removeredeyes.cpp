/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : a kipi plugin to automatically detect
 *               and remove red eyes from images
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

// C ANSI includes.

extern "C"
{
#include <unistd.h>
}

// KDE includes.

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes.

#include "removeredeyeswindow.h"

K_PLUGIN_FACTORY( RemoveRedEyesFactory, registerPlugin<Plugin_RemoveRedEyes>(); )
K_EXPORT_PLUGIN ( RemoveRedEyesFactory("kipiplugin_removeredeyes") )

Plugin_RemoveRedEyes::Plugin_RemoveRedEyes(QObject *parent, const QVariantList &/*args*/)
                    : KIPI::Plugin(RemoveRedEyesFactory::componentData(), parent, "RemoveRedEyes")
{
    kDebug(51001) << "Plugin_RemoveRedEyes plugin loaded" << endl;
}

void Plugin_RemoveRedEyes::setup( QWidget* widget )
{
    KIPI::Plugin::setup(widget);

    m_action = new KAction(KIcon("draw-eraser"), i18n("Remove Red-Eyes..."), actionCollection());
    m_action->setObjectName("removeredeyes");
    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(activate()));

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        m_action->setEnabled(false);
        return;
    }

    m_action->setEnabled(true);
    addAction(m_action);
}

void Plugin_RemoveRedEyes::activate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    m_dlg = new KIPIRemoveRedEyesPlugin::RemoveRedEyesWindow(interface, kapp->activeWindow());
    m_dlg->show();
}

KIPI::Category Plugin_RemoveRedEyes::category( KAction* action ) const
{
    if ( action == m_action )
        return KIPI::BatchPlugin;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::BatchPlugin; // no warning from compiler, please
}
