/* ============================================================
 * File  : plugin_mpegencoder.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-02
 * Description : Images MPEG encoder plugin ('kimg2mpeg'
 *               program from 'kvcdtools' project
 *               (http://kvcdtools.free.fr)
 *
 * Copyright 2003 by Gilles Caulier

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

// Include files for Qt

#include <qpushbutton.h>
#include <qgroupbox.h>

// Include files for KDE

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// Local includes

#include "checkbinprog.h"
#include "plugin_mpegencoder.h"
#include "kimg2mpg.h"

typedef KGenericFactory<Plugin_Mpegencoder> Factory;
K_EXPORT_COMPONENT_FACTORY( kipiplugin_mpegencoder,
                            Factory("kipiplugin_mpegencoder"));

// -----------------------------------------------------------

Plugin_Mpegencoder::Plugin_Mpegencoder(QObject *parent, const char*, const QStringList&)
    : KIPI::Plugin( Factory::instance(), parent, "MPEGEncoder")
{
    kdDebug( 51001 ) << "Plugin_Mpegencoder plugin loaded" << endl;

    KAction* action = new KAction (i18n("MPEG Encoder..."),
                                   "video",
                                   0,
                                   this,
                                   SLOT(slotActivate()),
                                   actionCollection(),
                                   "mpeg_encoder");
    addAction( action );
}

/////////////////////////////////////////////////////////////////////////////////////////////

Plugin_Mpegencoder::~Plugin_Mpegencoder()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_Mpegencoder::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
    KImg2mpgData *MPEGconverterDialog = new KImg2mpgData();
    MPEGconverterDialog->show();

    CheckBinProg* CheckExternalPrograms = new CheckBinProg(this);
    int ValRet = CheckExternalPrograms->findExecutables();

    if (ValRet == 0)
        MPEGconverterDialog->m_Encodebutton->setEnabled(false);

    if (ValRet == 2)
        MPEGconverterDialog->m_AudioInputFilename->setEnabled(false);

    KIPI::ImageCollection images = interface->currentSelection();
    if ( images.images().count() == 0 )
        images = interface->currentAlbum();

    // PENDING(blackie) extend this plugin to handle URLS rather than just strings
    MPEGconverterDialog->addItems( images.images().toStringList());
}

KIPI::Category Plugin_Mpegencoder::category() const
{
    return KIPI::TOOLSPLUGIN;
}




#include "plugin_mpegencoder.moc"
