/* ============================================================
 * File  : plugin_galleryexport.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-06
 * Description :
 *
 * Copyright 2004 by Renchi Raju

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

// KDE includes.
#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

// libkipi includes.
#include <libkipi/interface.h>

// Local includes.
#include "gallerywindow.h"
#include "plugin_galleryexport.h"

typedef KGenericFactory<Plugin_GalleryExport> Factory;

K_EXPORT_COMPONENT_FACTORY(kipiplugin_galleryexport,
                           Factory("kipiplugin_galleryexport"));

Plugin_GalleryExport::Plugin_GalleryExport(QObject *parent,
                                           const char*,
                                           const QStringList&)
    : KIPI::Plugin(Factory::instance(), parent, "GalleryExport")
{
    kdDebug(51001) << "Plugin_GalleryExport plugin loaded"
                   << endl;
}

void Plugin_GalleryExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);
    
    m_action = new KAction(i18n("Export to Remote Gallery..."),
                           "galleryexport",
                           0,
                           this,
                           SLOT(slotActivate()),
                           actionCollection(),
                           "galleryexport");

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    
    if (!interface) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    m_action->setEnabled(true);
    addAction(m_action);
}


Plugin_GalleryExport::~Plugin_GalleryExport()
{
}


void Plugin_GalleryExport::slotActivate()
{
    GalleryWindow dlg;
    dlg.exec();
}

KIPI::Category Plugin_GalleryExport::category( KAction* action ) const
{
    if (action == m_action)
        return KIPI::EXPORTPLUGIN;
    
    kdWarning(51000) << "Unrecognized action for plugin category identification"
                     << endl;
    return KIPI::EXPORTPLUGIN;
}


#include "plugin_galleryexport.moc"
