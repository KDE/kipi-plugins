/* ============================================================
 * File  : plugin_picasawebexport.cpp
 * Author: Vardhman Jain <vardhman @ gmail.com> 
 * Date  : 2007-17-07
 * Description :
 *
 * Copyright 2007 by Vardhman Jain <vardhman @ gmail.com>

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
extern "C"
{
	#include<unistd.h>
}
	
// KDE includes.
#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
// libkipi includes.
#include <libkipi/interface.h>

// Local includes.
#include "picasawebwindow.h"
#include "plugin_picasawebexport.h"

typedef KGenericFactory<Plugin_PicasawebExport> Factory;

K_EXPORT_COMPONENT_FACTORY(kipiplugin_picasawebexport,
                           Factory("kipiplugin_picasawebexport"))


Plugin_PicasawebExport::Plugin_PicasawebExport(QObject *parent,
                                           const char*,
                                           const QStringList&)
    : KIPI::Plugin(Factory::instance(), parent, "PicasawebExport")
{
    kdDebug(51001) << "Plugin_PicasawebExport plugin loaded"
                   << endl;
}


void Plugin_PicasawebExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);
    
    m_action = new KAction(i18n("Export to Picasaweb..."),
                           0,
                           this,
                           SLOT(slotActivate()),
                           actionCollection(),
                           "picasawebexport");

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    
    if (!interface) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        m_action->setEnabled(false);
        return;
    }

    m_action->setEnabled(true);
    addAction(m_action);
}


Plugin_PicasawebExport::~Plugin_PicasawebExport()
{
}


void Plugin_PicasawebExport::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }
	KStandardDirs dir;
	QString Tmp = dir.saveLocation("tmp", "kipi-picasawebexportplugin-" + QString::number(getpid()) + "/");
	
    dlg = new KIPIPicasawebExportPlugin::PicasawebWindow(interface,Tmp,kapp->activeWindow());
    dlg->show();
}

KIPI::Category Plugin_PicasawebExport::category( KAction* action ) const
{
    if (action == m_action)
        return KIPI::EXPORTPLUGIN;
    
    kdWarning(51000) << "Unrecognized action for plugin category identification"
                     << endl;
    return KIPI::EXPORTPLUGIN;
}


#include "plugin_picasawebexport.moc"
