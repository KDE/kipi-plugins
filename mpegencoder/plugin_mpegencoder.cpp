/* ============================================================
 * File  : plugin_mpegencoder.cpp
 * Author: Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date  : 2003-09-02
 * Description : Images MPEG encoder plugin ('kimg2mpeg'
 *               program from 'kvcdtools' project
 *               (http://kvcdtools.free.fr)
 *
 * Copyright 2003 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Include files for Qt

#include <qpushbutton.h>
#include <qgroupbox.h>

// Include files for KDE

#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// Local includes

#include "checkbinprog.h"
#include "kimg2mpg.h"
#include "plugin_mpegencoder.h"
#include "plugin_mpegencoder.moc"

typedef KGenericFactory<Plugin_Mpegencoder> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_mpegencoder,
                            Factory("kipiplugin_mpegencoder"))

Plugin_Mpegencoder::Plugin_Mpegencoder(QObject *parent, const char*, const QStringList&)
                  : KIPI::Plugin( Factory::instance(), parent, "MPEGEncoder")
{
    kdDebug( 51001 ) << "Plugin_Mpegencoder plugin loaded" << endl;
}

void Plugin_Mpegencoder::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_actionMPEGEncoder = new KAction (i18n("Create MPEG Slide Show..."),
                                       "video",
                                       0,
                                       this,
                                       SLOT(slotActivate()),
                                       actionCollection(),
                                       "mpeg_encoder");

    addAction( m_actionMPEGEncoder );
}

Plugin_Mpegencoder::~Plugin_Mpegencoder()
{
}

void Plugin_Mpegencoder::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !interface )
    {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
    }

    KIPIMPEGEncoderPlugin::KImg2mpgData *MPEGconverterDialog =
                           new KIPIMPEGEncoderPlugin::KImg2mpgData( interface, kapp->activeWindow() );

    KIPIMPEGEncoderPlugin::CheckBinProg* CheckExternalPrograms =
                           new KIPIMPEGEncoderPlugin::CheckBinProg(this);

    int ValRet = CheckExternalPrograms->findExecutables();

    MPEGconverterDialog->show();

    if (ValRet == 0)
        MPEGconverterDialog->m_Encodebutton->setEnabled(false);

    if (ValRet == 2)
        MPEGconverterDialog->m_AudioInputFilename->setEnabled(false);

    KIPI::ImageCollection images = interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    // PENDING(blackie) extend this plugin to handle URLS rather than just strings
    MPEGconverterDialog->addItems( images.images().toStringList());
}

KIPI::Category Plugin_Mpegencoder::category( KAction* action ) const
{
    if ( action == m_actionMPEGEncoder )
       return KIPI::EXPORTPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::TOOLSPLUGIN; // no warning from compiler, please
}
