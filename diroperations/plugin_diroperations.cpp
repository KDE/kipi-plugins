#include <kurl.h>
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

// Local includes

 #include "plugin_diroperations.h"

typedef KGenericFactory<Plugin_DirOperations> Factory;
 K_EXPORT_COMPONENT_FACTORY( kipiplugin_diroperations,
                             Factory("kipiplugin_diroperations"));

 /////////////////////////////////////////////////////////////////////////////////////////////////////

 Plugin_DirOperations::Plugin_DirOperations(QObject *parent, const char*, const QStringList&)
                       : KIPI::Plugin( Factory::instance(), parent, "MiscsOperations")
 {
    kdDebug( 51001 ) << "Plugin_DirOperations plugin loaded" << endl;
}

void Plugin_DirOperations::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    m_action_OpenIn = new KActionMenu(i18n("&Open Album in ..."),
                         actionCollection(),
                         "miscoperations_open_in");

    m_action_OpenIn->insert(new KAction ("Konqueror",
                         0,
                         this,
                         SLOT(slotOpenInKonqui()),
                         actionCollection(),
                         "miscoperations_open_in_konqui"));

    m_action_OpenIn->insert(new KAction ("Nautilus",
                         0,
                         this,
                         SLOT(slotOpenInNautilus()),
                         actionCollection(),
                         "miscoperations_open_in_nautilus"));

    addAction( m_action_OpenIn );

#ifdef TEMPORARILY_REMOVED
    m_action_OpenIn->setEnabled(false);

    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumItemsSelected(bool)),
            SLOT(slotItemsSelected(bool)));

    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumCurrentChanged(Digikam::AlbumInfo *)),
            SLOT(slotAlbumSelected(Digikam::AlbumInfo *)));
#endif
 }


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_DirOperations::slotOpenInKonqui()
{
    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*>( parent() );
    KIPI::ImageCollection images = interface->currentAlbum();
    if (images.images().count() == 0) return;

    new KRun(images.path()); // KRun will delete itself.
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_DirOperations::slotOpenInNautilus()
{
    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*>( parent() );
    KIPI::ImageCollection images = interface->currentAlbum();

   if (images.images().count() == 0) return;

   m_browserProc = new KProcess;

   *m_browserProc << "nautilus";
   *m_browserProc << images.path().url();


   if (m_browserProc->start() == false)
      KMessageBox::error(0, i18n("Cannot start 'nautilus' filemanager.\n"
                                 "Please, check your installation!"));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEMPORARILY_REMOVED
void Plugin_DirOperations::slotAlbumSelected(Digikam::AlbumInfo *album)
{
  m_action_OpenIn->setEnabled((album!=NULL));
}
#endif


KIPI::Category  Plugin_DirOperations::category() const
{
    return KIPI::IMAGESPLUGIN;
}

#include "plugin_diroperations.moc"
