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

K_EXPORT_COMPONENT_FACTORY( kipiplugin_timeadjust,
                            KGenericFactory<Plugin_TimeAdjust>("kipiplugin_timeadjust"));

Plugin_TimeAdjust::Plugin_TimeAdjust(QObject *parent,
                                     const char*,
                                     const QStringList&)
    : KIPI::Plugin(parent, "TimeAdjust"), m_dialog( 0 )
{
    // Insert our translations into the global catalogue
    KGlobal::locale()->insertCatalogue("digikamplugin_timeadjust");

    kdDebug( 51001 ) << "Plugin_TimeAdjust plugin loaded" << endl;

    // this is our action shown in the menubar/toolbar of the mainwindow
    (void) new KAction (i18n("Adjust Time and Date"),
                        "",
                        0,	// or a shortcut like CTRL+SHIFT+Key_S,
                        this,
                        SLOT(slotActivate()),
                        actionCollection(),
                        "timeadjust");

    m_interface = dynamic_cast< KIPI::Interface* >( parent );
}

void Plugin_TimeAdjust::slotActivate()
{
    // Get the current/selected album
    KIPI::ImageCollection images = m_interface->currentSelection();
    if ( images.images().count() == 0 )
        images = m_interface->currentAlbum();

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

