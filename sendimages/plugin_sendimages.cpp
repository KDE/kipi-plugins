/* ============================================================
 * File  : plugin_sendimages.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-11-04
 * Description : KIPI Send Mail Images Plugin.
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

// KIPI includes

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
    KIPI::ImageCollection images = interface->currentSelection();
    if ( !images.isValid() )
        images = interface->currentAlbum();
    if ( !images.isValid() )
        return;

    KStandardDirs dir;
    QString Tmp = dir.saveLocation("tmp", "kipi-sendimages-" + QString::number(getpid()) + "/");

    m_sendImagesDialog = new SendImagesDialog(0, Tmp, interface, images);
    m_sendImagesDialog->show();
}

KIPI::Category Plugin_SendImages::category() const
{
    return KIPI::IMAGESPLUGIN;
}

#include "plugin_sendimages.moc"
