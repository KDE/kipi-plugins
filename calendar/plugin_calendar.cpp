/* ============================================================
 * File  : plugin_jpeglossless.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.
  
#include <qprogressdialog.h>
#include <qtimer.h> 
 
// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kurl.h>
#include <kapplication.h>

// Local includes.

#include "plugin_calendar.h"
#include "calwizard.h"

typedef KGenericFactory<Plugin_Calendar> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_calendar,
                            Factory("kipiplugin_calendar"));

Plugin_Calendar::Plugin_Calendar(QObject *parent,
                                 const char*,
                                 const QStringList &)
    : KIPI::Plugin(Factory::instance(), parent, "Calendar")
{
    kdDebug( 51001 ) << "Loaded Plugin_Calendar" << endl;
}

void Plugin_Calendar::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    m_calendarAction = new KAction(i18n("Create Calendar..."),
                                   "date",
                                   0,
                                   this,
                                   SLOT(slotActivate()),
                                   actionCollection(),
                                   "calendar");
    
    addAction( m_calendarAction );
}

Plugin_Calendar::~Plugin_Calendar()
{
}

void Plugin_Calendar::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
    DKCalendar::CalWizard* w = new DKCalendar::CalWizard( interface, kapp->activeWindow() );
    w->show();
}

KIPI::Category Plugin_Calendar::category( KAction* action ) const
{
    if ( action == m_calendarAction )
       return KIPI::TOOLSPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::TOOLSPLUGIN; // no warning from compiler, please            
}

#include "plugin_calendar.moc"
