/* ============================================================
 * File   : plugin_miscsoperations.cpp
 *
 * Authors: Gregory KOKANOSKY <gregory dot kokanosky at free.fr>
 *          Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Date   : 01/2004
 *
 * Description : Miscs operations plugin parts for Digikam
 *               - KDE Gamma correction call.
 *               - Images to KDE desktop.
 *               - Tip of day.
 *               - Open Album in Konqueror/Nautilus
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
 #include <krun.h>
 #include <kapplication.h>
 #include <kmessagebox.h>
 #include <ktextbrowser.h>

// Digikam includes

 #include <digikam/albummanager.h>
 #include <digikam/albuminfo.h>

// Local includes

 #include "plugin_miscsoperations.h"

 K_EXPORT_COMPONENT_FACTORY( kipiplugin_miscsoperations,
                             KGenericFactory<Plugin_MiscsOperations>("kipiplugin_miscsoperations"));

 /////////////////////////////////////////////////////////////////////////////////////////////////////

 Plugin_MiscsOperations::Plugin_MiscsOperations(QObject *parent, const char*, const QStringList&)
                       : KIPI::Plugin(parent, "MiscsOperations")
 {
    KGlobal::locale()->insertCatalogue("kipiplugin_miscsoperations");

    kdDebug( 51001 ) << "Plugin_MiscsOperations plugin loaded" << endl;

    m_action_OpenIn = new KActionMenu(i18n("&Open Album in ..."),
                         actionCollection(),
                         "miscoperations_open_in");

    m_action_OpenIn->insert(new KAction ("Konqueror",
                         0,
                         this,
                         SLOT(slotOpenInKonqui()),
                         m_action_OpenIn,
                         "miscoperations_open_in_konqui"));

    m_action_OpenIn->insert(new KAction ("Nautilus",
                         0,
                         this,
                         SLOT(slotOpenInNautilus()),
                         m_action_OpenIn,
                         "miscoperations_open_in_nautilus"));


    m_action_Background = new KActionMenu(i18n("&Set as Background"),
                         actionCollection(),
                         "images2desktop");

    m_action_Background->insert(new KAction (i18n("Centered"),
                         0,
                         this,
                         SLOT(slotSetCenter()),
                         m_action_Background,
                         "images2desktop_center"));

    m_action_Background->insert(new KAction (i18n("Tiled"),
                         0,
                         this,
                         SLOT(slotSetTiled()),
                         m_action_Background,
                         "images2desktop_tiled"));

    m_action_Background->insert(new KAction (i18n("Centered Tiled"),
                         0,
                         this,
                         SLOT(slotSetCenterTiled()),
                         m_action_Background,
                         "images2desktop_center_tiled"));

    m_action_Background->insert(new KAction (i18n("Centered Max-Aspect"),
                         0,
                         this,
                         SLOT(slotSetCenteredMaxpect()),
                         m_action_Background,
                         "images2desktop_center_maxpect"));

    m_action_Background->insert(new KAction (i18n("Tiled Max-Aspect"),
                         0,
                         this,
                         SLOT(slotSetTiledMaxpect()),
                         m_action_Background,
                         "images2desktop_tiled_maxpect"));

    m_action_Background->insert(new KAction (i18n("Scaled"),
                         0,
                         this,
                         SLOT(slotSetScaled()),
                         m_action_Background,
                         "images2desktop_scaled"));

    m_action_Background->insert(new KAction (i18n("Centered Auto Fit"),
                         0,
                         this,
                         SLOT(slotSetCenteredAutoFit()),
                         m_action_Background,
                         "images2desktop_centered_auto_fit"));

    m_action_gammaCorrection = new KAction (i18n("Gamma correction..."),
                         "kgamma",
                         0,
                         this,
                         SLOT(slotGammaCorrection()),
                         actionCollection(),
                         "gamma_correction");

    m_action_Background->setEnabled(false);
    m_action_OpenIn->setEnabled(false);

    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumItemsSelected(bool)),
            SLOT(slotItemsSelected(bool)));

    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumCurrentChanged(Digikam::AlbumInfo *)),
            SLOT(slotAlbumSelected(Digikam::AlbumInfo *)));
 }


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotSetCenter()
{
   return setWallpaper(CENTER);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotSetTiled()
{
   return setWallpaper(TILED);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotSetCenterTiled()
{
   return  setWallpaper(CENTER_TILED);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotSetCenteredMaxpect()
{
   return setWallpaper(CENTER_MAXPECT);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotSetTiledMaxpect()
{
   return setWallpaper(TILED_MAXPECT);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotSetScaled()
{
   return setWallpaper(SCALED);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotSetCenteredAutoFit()
{
   return setWallpaper(CENTERED_AUTOFIT);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::setWallpaper(int layout)
{
   if (layout>CENTERED_AUTOFIT || layout < CENTER)
      return;

   Digikam::AlbumInfo *album =
      Digikam::AlbumManager::instance()->currentAlbum();

   if (!album) return;

   QString cmd = QString("dcop kdesktop KBackgroundIface setWallpaper '%1' %2")
                         .arg(album->getSelectedItemsPath().first()).arg(layout);

   KRun::runCommand(cmd);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotOpenInKonqui()
{
   Digikam::AlbumInfo *album =
        Digikam::AlbumManager::instance()->currentAlbum();

   if (!album) return;

   kapp->invokeBrowser(album->getPath());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotOpenInNautilus()
{
   Digikam::AlbumInfo *album =
      Digikam::AlbumManager::instance()->currentAlbum();

   if (!album) return;

   m_browserProc = new KProcess;

   *m_browserProc << "nautilus";
   *m_browserProc << album->getPath();


   if (m_browserProc->start() == false)
      KMessageBox::error(0, i18n("Cannot start 'nautilus' filemanager.\n"
                                 "Please, check your installation!"));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotGammaCorrection(void)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotItemsSelected(bool val)
{
   m_action_Background->setEnabled(val);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_MiscsOperations::slotAlbumSelected(Digikam::AlbumInfo *album)
{
  m_action_OpenIn->setEnabled((album!=NULL));
}


KIPI::Category  Plugin_MiscsOperations::category() const
{
    return KIPI::IMAGESPLUGIN;
}
