/* ============================================================
 * File  : plugin_cdarchiving.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-05
 * Description : Albums CD archiving Digikam plugin
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
 *
 * ============================================================ */

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

#include "cdarchiving.h"
#include "plugin_cdarchiving.h"


K_EXPORT_COMPONENT_FACTORY( kipiplugin_cdarchiving,
                            KGenericFactory<Plugin_CDArchiving>("kipiplugin_cdarchiving"));

// -----------------------------------------------------------
Plugin_CDArchiving::Plugin_CDArchiving(QObject *parent, const char*, const QStringList&)
            : KIPI::Plugin(parent, "CDArchiving")
{
    KGlobal::locale()->insertCatalogue("kipiplugin_cdarchiving");
    kdDebug( 51001 ) << "Plugin_CDArchiving plugin loaded" << endl;

    m_action_cdarchiving = new KAction (i18n("Archive to CD ..."),        // Menu message.
                                        "cd",                             // Menu icon.
                                        0,
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "cd_archiving");

    m_cdarchiving = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_CDArchiving::~Plugin_CDArchiving()
{
    // No need to delete m_cdarchiving as its a QObject child of this
    // and will be deleted automatically
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_CDArchiving::slotActivate()
{
    if (!m_cdarchiving) {
        m_cdarchiving = new CDArchiving( dynamic_cast<KIPI::Interface*>( parent() ), this, m_action_cdarchiving);
    }
    m_cdarchiving->Activate();
}

KIPI::Category Plugin_CDArchiving::category() const
{
    return KIPI::IMAGESPLUGIN;
}

#include "plugin_cdarchiving.moc"
