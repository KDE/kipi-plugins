/* ============================================================
 * File  : plugin_cdarchiving.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-05
 * Description : Albums CD archiving KIPI plugin
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

// KDE includes. 
 
#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

// Local includes.

#include "cdarchiving.h"
#include "plugin_cdarchiving.h"

typedef KGenericFactory<Plugin_CDArchiving> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_cdarchiving,
                            Factory("kipiplugin_cdarchiving"));

// -----------------------------------------------------------
Plugin_CDArchiving::Plugin_CDArchiving(QObject *parent, const char*, const QStringList&)
            : KIPI::Plugin( Factory::instance(), parent, "CDArchiving")
{
    kdDebug( 51001 ) << "Plugin_CDArchiving plugin loaded" << endl;
}

void Plugin_CDArchiving::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    m_action_cdarchiving = new KAction (i18n("Archive to CD ..."),        // Menu message.
                                        "cd",                             // Menu icon.
                                        0,
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "cd_archiving");
    
    addAction( m_action_cdarchiving );
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
    if (!m_cdarchiving) 
        {
        m_cdarchiving = new KIPICDArchivingPlugin::CDArchiving( 
                            dynamic_cast<KIPI::Interface*>( parent() ),
                            this, m_action_cdarchiving);
        }
        
    m_cdarchiving->Activate();
}

KIPI::Category Plugin_CDArchiving::category( KAction* action ) const
{
    if ( action == m_action_cdarchiving )
       return KIPI::EXPORTPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;    
    return KIPI::EXPORTPLUGIN; // no warning from compiler, please    
}


#include "plugin_cdarchiving.moc"
