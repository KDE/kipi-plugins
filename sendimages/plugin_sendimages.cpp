/* ============================================================
 * File  : plugin_sendimages.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-11-04
 * Description : KIPI E-Mail Images Plugin.
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

 // C Ansi includes

extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <qimage.h>

// Include files for KDE

#include <klocale.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kscan.h>
#include <kimageio.h>

// Local includes

#include "sendimagesdialog.h"
#include "plugin_sendimages.h"

typedef KGenericFactory<Plugin_SendImages> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_sendimages,
                            Factory("kipiplugin_sendimages"));

// -----------------------------------------------------------
Plugin_SendImages::Plugin_SendImages(QObject *parent, const char*, const QStringList&)
                 : KIPI::Plugin( Factory::instance(), parent, "SendImages")
{
    kdDebug( 51001 ) << "Plugin_SendImages plugin loaded" << endl;

}

void Plugin_SendImages::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    
    m_action_sendimages = new KAction (i18n("E-mail Images..."),     // Menu message.
                                        "mail_new",                  // Menu icon.
                                        0,
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "send_images");
    addAction( m_action_sendimages );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_SendImages::~Plugin_SendImages()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_SendImages::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
    
    if ( !interface ) 
       {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
       }

    KIPI::ImageCollection images = interface->currentScope();
    
    if ( !images.isValid() )
        return;

    KStandardDirs dir;
    QString Tmp = dir.saveLocation("tmp", "kipi-sendimagesplugin-" + QString::number(getpid()) + "/");

    m_sendImagesDialog = new KIPISendimagesPlugin::SendImagesDialog(0, Tmp, interface, images);
    m_sendImagesDialog->show();
}

KIPI::Category Plugin_SendImages::category( KAction* action ) const
{
    if ( action == m_action_sendimages )
       return KIPI::IMAGESPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}

#include "plugin_sendimages.moc"
