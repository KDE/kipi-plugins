/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

namespace KIPICalendarPlugin
{

K_PLUGIN_FACTORY(CalendarFactory, registerPlugin<Plugin_Calendar>();)
K_EXPORT_PLUGIN(CalendarFactory("kipiplugin_calendar"))

Plugin_Calendar::Plugin_Calendar(QObject* const parent, const QVariantList&)
    : Plugin(CalendarFactory::componentData(), parent, "Calendar")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Calendar plugin loaded";
}

Plugin_Calendar::~Plugin_Calendar()
{
}

void Plugin_Calendar::setup(QWidget* widget)
{
    Plugin::setup(widget);

    m_actionCalendar = actionCollection()->addAction("calendar");
    m_actionCalendar->setText(i18n("Create Calendar..."));
    m_actionCalendar->setIcon(KIcon("view-pim-calendar"));

    connect(m_actionCalendar, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_actionCalendar);
}

void Plugin_Calendar::slotActivate()
{
    CalWizard w(kapp->activeWindow());
    w.exec();
}

Category Plugin_Calendar::category(KAction* action) const
{
    if (action == m_actionCalendar)
    {
        return ToolsPlugin;
    }

    kWarning() << "Unrecognized action for plugin category identification";
    return ToolsPlugin; // no warning from compiler, please
}

}  // NameSpace KIPICalendarPlugin
