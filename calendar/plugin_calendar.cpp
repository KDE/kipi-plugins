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

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kurl.h>
#include <kmessagebox.h>

#include <qprogressdialog.h>
#include <qtimer.h>

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
    KAction* action = new KAction(i18n("Create Calendar..."),
                                  "date",
				  0,
				  this,
                                  SLOT(slotActivate()),
                                  actionCollection(),
                                  "calendar");
    addAction( action );
}

Plugin_Calendar::~Plugin_Calendar()
{
}

void Plugin_Calendar::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
    DKCalendar::CalWizard* w = new DKCalendar::CalWizard( interface );
    w->show();
}

KIPI::Category Plugin_Calendar::category() const
{
    return KIPI::TOOLSPLUGIN;
}

#include "plugin_calendar.moc"
