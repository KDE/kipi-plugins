/* ============================================================
 * File   : plugin_wallpaper.cpp
 *
 * Authors: Gregory KOKANOSKY <gregory dot kokanosky at free.fr>
 *          Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Date   : 01/2004
 *
 * Description : Set Wall paper plugin for KIPI
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
 #include <kdeversion.h>
 #include <kfiledialog.h>
 #include <kio/netaccess.h>

// KIPI includes

 #include <libkipi/interface.h>
 #include <libkipi/imagecollection.h>

// Local includes

 #include "plugin_wallpaper.h"

typedef KGenericFactory<Plugin_WallPaper> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_wallpaper,
                            Factory("kipiplugin_wallpaper"));

 /////////////////////////////////////////////////////////////////////////////////////////////////////

 Plugin_WallPaper::Plugin_WallPaper(QObject *parent, const char*, const QStringList&)
                       : KIPI::Plugin( Factory::instance(), parent, "WallPaper")
 {
     kdDebug( 51001 ) << "Plugin_WallPaper plugin loaded" << endl;
 }

void Plugin_WallPaper::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    
    m_action_Background = new KActionMenu(i18n("&Set as Background"),
                         actionCollection(),
                         "images2desktop");

    m_action_Background->insert(new KAction (i18n("Centered"),
                         0,
                         this,
                         SLOT(slotSetCenter()),
                         actionCollection(),
                         "images2desktop_center"));

    m_action_Background->insert(new KAction (i18n("Tiled"),
                         0,
                         this,
                         SLOT(slotSetTiled()),
                         actionCollection(),
                         "images2desktop_tiled"));

    m_action_Background->insert(new KAction (i18n("Centered Tiled"),
                         0,
                         this,
                         SLOT(slotSetCenterTiled()),
                         actionCollection(),
                         "images2desktop_center_tiled"));

    m_action_Background->insert(new KAction (i18n("Centered Max-Aspect"),
                         0,
                         this,
                         SLOT(slotSetCenteredMaxpect()),
                         actionCollection(),
                         "images2desktop_center_maxpect"));

    m_action_Background->insert(new KAction (i18n("Tiled Max-Aspect"),
                         0,
                         this,
                         SLOT(slotSetTiledMaxpect()),
                         actionCollection(),
                         "images2desktop_tiled_maxpect"));

    m_action_Background->insert(new KAction (i18n("Scaled"),
                         0,
                         this,
                         SLOT(slotSetScaled()),
                         actionCollection(),
                         "images2desktop_scaled"));

    m_action_Background->insert(new KAction (i18n("Centered Auto Fit"),
                         0,
                         this,
                         SLOT(slotSetCenteredAutoFit()),
                         actionCollection(),
                         "images2desktop_centered_auto_fit"));

    addAction( m_action_Background );

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
    
    if ( !interface ) 
           {
           kdError( 51000 ) << "Kipi interface is null!" << endl;
           return;
           }
    
    KIPI::ImageCollection selection = interface->currentSelection();
    m_action_Background->setEnabled( selection.isValid() );

    connect( interface, SIGNAL(selectionChanged(bool)), 
             m_action_Background, SLOT(setEnabled(bool)));
 }


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_WallPaper::slotSetCenter()
{
   return setWallpaper(CENTER);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_WallPaper::slotSetTiled()
{
   return setWallpaper(TILED);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_WallPaper::slotSetCenterTiled()
{
   return  setWallpaper(CENTER_TILED);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_WallPaper::slotSetCenteredMaxpect()
{
   return setWallpaper(CENTER_MAXPECT);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_WallPaper::slotSetTiledMaxpect()
{
   return setWallpaper(TILED_MAXPECT);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_WallPaper::slotSetScaled()
{
   return setWallpaper(SCALED);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_WallPaper::slotSetCenteredAutoFit()
{
   return setWallpaper(CENTERED_AUTOFIT);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_WallPaper::setWallpaper(int layout)
{
   if (layout>CENTERED_AUTOFIT || layout < CENTER)
      return;

   KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
   
   if ( !interface ) 
           {
           kdError( 51000 ) << "Kipi interface is null!" << endl;
           return;
           }
   
   KIPI::ImageCollection images = interface->currentSelection();

   if (!images.isValid() ) return;

   KURL url=images.images()[0];
   QString path;
   if (url.isLocalFile()) {
      path=url.path();
   } else {
      
      // PENDING We need a way to get a parent widget
      // Sun, 06 Jun 2004 - Aurélien
      
      KMessageBox::information( 0L, i18n(
         "<qt><p>You selected a remote image. It needs to be saved to your local disk to be used as a wallpaper.</p><p>You will now be asked where to save the image.</p</qt>"
         ));
      path=KFileDialog::getSaveFileName(url.fileName(), QString::null, 0L);
      if (path.isNull()) return;
#if KDE_VERSION > 0x30200
      KIO::NetAccess::download(url, path, 0L);
#else
      KIO::NetAccess::download(url, path);
#endif
   }

   QString cmd = QString("dcop kdesktop KBackgroundIface setWallpaper '%1' %2")
                         .arg(path).arg(layout);

   KRun::runCommand(cmd);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

KIPI::Category  Plugin_WallPaper::category( KAction* action ) const
{
    if ( action == m_action_Background )
       return KIPI::IMAGESPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}

#include "plugin_wallpaper.moc"
