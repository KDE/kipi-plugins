/* ============================================================
 * File  : plugin_timeadjust.cpp
 * Author: Jesper K. Pedersen <blackie@blackie.dk>
 * Date  : 2004-05-16
 * Description :
 *
 * Copyright 2003 by Jesper K. Pedersen
 *
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
 * ============================================================ */

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

#include "plugin_timeadjust.h"
#include <libkipi/imagecollection.h>
#include "timeadjustdialog.h"

typedef KGenericFactory<Plugin_TimeAdjust> Factory;
K_EXPORT_COMPONENT_FACTORY( kipiplugin_timeadjust,
                            Factory("kipiplugin_timeadjust"));

Plugin_TimeAdjust::Plugin_TimeAdjust(QObject *parent,
                                     const char*,
                                     const QStringList&)
    : KIPI::Plugin( Factory::instance(), parent, "TimeAdjust"), m_dialog( 0 )
{
    // Insert our translations into the global catalogue
    kdDebug( 51001 ) << "Plugin_TimeAdjust plugin loaded" << endl;

}

void Plugin_TimeAdjust::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    // this is our action shown in the menubar/toolbar of the mainwindow
    KAction* action = new KAction (i18n("Adjust Time and Date"),
                                   "",
                                   0,	// or a shortcut like CTRL+SHIFT+Key_S,
                                   this,
                                   SLOT(slotActivate()),
                                   actionCollection(),
                                   "timeadjust");
    addAction( action );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
}

void Plugin_TimeAdjust::slotActivate()
{
    // Get the current/selected album
    KIPI::ImageCollection images = m_interface->currentScope();
    if ( !images.isValid() )
        return;

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
    if ( m_dialog == 0 )
        m_dialog = new TimeAdjustDialog( interface, 0, "time adjust dialog" );
    m_dialog->setImages( images.images() );
    m_dialog->show();
}

KIPI::Category Plugin_TimeAdjust::category() const
{
    return KIPI::IMAGESPLUGIN;
}


#include "plugin_timeadjust.moc"
