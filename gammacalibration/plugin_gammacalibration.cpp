/* ============================================================
 * File   : plugin_gammacalibration.cpp
 *
 * Authors: Gregory KOKANOSKY <gregory dot kokanosky at free.fr>
 *          Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Date   : 01/2004
 *
 * Description : KDE Gamma correction call.
 *               
 * Copyright 2004 by Gregory KOKANOSKY and Gilles CAULIER
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

// Include files for KDE

 #include <klocale.h>
 #include <kaction.h>
 #include <kgenericfactory.h>
 #include <klibloader.h>
 #include <kconfig.h>
 #include <kdebug.h>
 #include <kapplication.h>
 #include <kmessagebox.h>
 
// Local includes

 #include "plugin_gammacalibration.h"

typedef KGenericFactory<Plugin_GammaCalibration> Factory;
 K_EXPORT_COMPONENT_FACTORY( kipiplugin_gammacalibration,
                             Factory("kipiplugin_gammacalibration"));

 /////////////////////////////////////////////////////////////////////////////////////////////////////

 Plugin_GammaCalibration::Plugin_GammaCalibration(QObject *parent, const char*, const QStringList&)
                        : KIPI::Plugin( Factory::instance(), parent, "GammaCalibration")
{
    kdDebug( 51001 ) << "Plugin_GammaCalibration plugin loaded" << endl;
}

void Plugin_DiPlugin_GammaCalibration::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_gammaCorrection = new KAction (i18n("Gamma correction..."),
                         "kgamma",
                         0,
                         this,
                         SLOT(slotGammaCorrection()),
                         actionCollection(),
                         "gamma_correction");    
}

void Plugin_GammaCalibration::slotGammaCorrection(void)
{
   QStringList args;
   QString *perror = 0;
   int *ppid = 0;

   args << "kgamma";
   int ValRet = KApplication::kdeinitExec(QString::fromLatin1("kcmshell"), args, perror, ppid);

   if ( ValRet != 0 )
     KMessageBox::error(0, i18n("Cannot start \"KGamma\" extension in KDE control centrer!\n"
                                "Please check your installation."));
}

KIPI::Category Plugin_GammaCalibration::category() const
{
    return KIPI::TOOLPLUGIN;
}

#include "plugin_gammacalibration.moc"
