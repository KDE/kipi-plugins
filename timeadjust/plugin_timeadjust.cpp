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

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>

// Local includes.

#include "timeadjustdialog.h"
#include "plugin_timeadjust.h"

typedef KGenericFactory<Plugin_TimeAdjust> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_timeadjust,
                            Factory("kipiplugin_timeadjust"));

Plugin_TimeAdjust::Plugin_TimeAdjust(QObject *parent,
                                     const char*,
                                     const QStringList&)
    : KIPI::Plugin( Factory::instance(), parent, "TimeAdjust"), m_dialog( 0 )
{
    kdDebug( 51001 ) << "Plugin_TimeAdjust plugin loaded" << endl;
}

void Plugin_TimeAdjust::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    // this is our action shown in the menubar/toolbar of the mainwindow

    m_actionTimeAjust = new KAction (i18n("Adjust Time && Date..."),
                                     "clock",
                                     0,     // or a shortcut like CTRL+SHIFT+Key_S,
                                     this,
                                     SLOT(slotActivate()),
                                     actionCollection(),
                                     "timeadjust");

    addAction( m_actionTimeAjust );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface )
       {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
       }

    KIPI::ImageCollection selection = m_interface->currentScope();
    m_actionTimeAjust->setEnabled( selection.isValid() );

    connect( m_interface, SIGNAL(currentScopeChanged(bool)),
             m_actionTimeAjust, SLOT(setEnabled(bool)));
}

void Plugin_TimeAdjust::slotActivate()
{
    // Get the current/selected album
    KIPI::ImageCollection images = m_interface->currentScope();

    if ( !images.isValid() )
        return;

    if ( m_dialog == 0 )
        m_dialog = new KIPITimeAdjustPlugin::TimeAdjustDialog( m_interface, 0, "time adjust dialog" );

    m_dialog->setImages( images.images() );
    m_dialog->show();
}

KIPI::Category Plugin_TimeAdjust::category( KAction* action ) const
{
    if ( action == m_actionTimeAjust )
       return KIPI::IMAGESPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}


#include "plugin_timeadjust.moc"
