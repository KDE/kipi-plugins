/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-11-03
 * Description : plugin entry point.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
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

#include "plugin_calendar.h"
#include "plugin_calendar.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "calwizard.h"

K_PLUGIN_FACTORY( CalendarFactory, registerPlugin<Plugin_Calendar>(); )
K_EXPORT_PLUGIN ( CalendarFactory("kipiplugin_calendar") )

Plugin_Calendar::Plugin_Calendar(QObject *parent, const QVariantList&)
               : KIPI::Plugin(CalendarFactory::componentData(), parent, "Calendar")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Calendar plugin loaded";
}

Plugin_Calendar::~Plugin_Calendar()
{
}

void Plugin_Calendar::setup( QWidget* widget )
{
    KIPI::Plugin::setup(widget);

    m_actionCalendar = actionCollection()->addAction("calendar");
    m_actionCalendar->setText(i18n("Create Calendar..."));
    m_actionCalendar->setIcon(KIcon("view-pim-calendar"));

    connect(m_actionCalendar, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_actionCalendar);

    m_interface = dynamic_cast< KIPI::Interface* >(parent());
    if (!m_interface)
    {
       kError() << "Kipi interface is null!";
       return;
    }
}

void Plugin_Calendar::slotActivate()
{
    KIPICalendarPlugin::CalWizard w( m_interface, kapp->activeWindow() );
    w.exec();
}

KIPI::Category Plugin_Calendar::category( KAction* action ) const
{
    if ( action == m_actionCalendar )
       return KIPI::ToolsPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ToolsPlugin; // no warning from compiler, please
}
