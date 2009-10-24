/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-01-01
 * Description : Wall Paper kipi-plugin
 *
 * Copyright (C) 2004      by Gregory Kokanosky <gregory dot kokanosky at free.fr>
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "plugin_wallpaper.h"
#include "plugin_wallpaper.moc"

// KDE includes

#include <klocale.h>
#include <kactionmenu.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kdebug.h>
#include <krun.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <ktextbrowser.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kdesktop_interface.h"

K_PLUGIN_FACTORY( WallPaperFactory, registerPlugin<Plugin_WallPaper>(); )
K_EXPORT_PLUGIN ( WallPaperFactory("kipiplugin_wallpaper") )

Plugin_WallPaper::Plugin_WallPaper(QObject *parent, const QVariantList&)
                : KIPI::Plugin(WallPaperFactory::componentData(), parent, "WallPaper")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_WallPaper plugin loaded";
}

void Plugin_WallPaper::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_actionBackground = new KActionMenu(i18n("&Set as Background"),
                         actionCollection());
    m_actionBackground->setObjectName("images2desktop");

    KAction *centered = actionCollection()->addAction("images2desktop_center");
    centered->setText(i18n("Centered"));
    connect(centered, SIGNAL(triggered(bool)),
            this, SLOT(slotSetCenter()));
    m_actionBackground->addAction(centered);

    KAction *centeredTiled = actionCollection()->addAction("images2desktop_center_tiled");
    centeredTiled->setText(i18n("Centered Tiled"));
    connect(centeredTiled, SIGNAL(triggered(bool)),
            this, SLOT(slotSetCenterTiled()));
    m_actionBackground->addAction(centeredTiled);

    KAction *centeredMax = actionCollection()->addAction("images2desktop_center_maxpect");
    centeredMax->setText(i18n("Centered Max-Aspect"));
    connect(centeredMax, SIGNAL(triggered(bool)),
            this, SLOT(slotSetCenteredMaxpect()));
    m_actionBackground->addAction(centeredMax);

    KAction *tiledMax = actionCollection()->addAction("images2desktop_tiled_maxpect");
    tiledMax->setText(i18n("Tiled Max-Aspect"));
    connect(tiledMax, SIGNAL(triggered(bool)),
            this, SLOT(slotSetTiledMaxpect()));
    m_actionBackground->addAction(tiledMax);

    KAction *scaled = actionCollection()->addAction("images2desktop_scaled");
    scaled->setText(i18n("Scaled"));
    connect(scaled, SIGNAL(triggered(bool)),
            this, SLOT(slotSetScaled()));
    m_actionBackground->addAction(scaled);

    KAction *centeredAutoFit = actionCollection()->addAction("images2desktop_centered_auto_fit");
    centeredAutoFit->setText(i18n("Centered Auto Fit"));
    connect(centeredAutoFit, SIGNAL(triggered(bool)),
            this, SLOT(slotSetCenteredAutoFit()));
    m_actionBackground->addAction(centeredAutoFit);

    KAction *scaleCrop = actionCollection()->addAction("images2desktop_scale_and_crop");
    scaleCrop->setText(i18n("Scale && Crop"));
    connect(scaleCrop, SIGNAL(triggered(bool)),
            this, SLOT(slotSetScaleAndCrop()));
    m_actionBackground->addAction(scaleCrop);

    addAction( m_actionBackground );

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KIPI::ImageCollection selection = interface->currentSelection();
    m_actionBackground->setEnabled( selection.isValid() );

    connect( interface, SIGNAL(selectionChanged(bool)),
             m_actionBackground, SLOT(setEnabled(bool)));
 }

void Plugin_WallPaper::slotSetCenter()
{
   return setWallpaper(CENTER);
}

void Plugin_WallPaper::slotSetTiled()
{
   return setWallpaper(TILED);
}

void Plugin_WallPaper::slotSetCenterTiled()
{
   return  setWallpaper(CENTER_TILED);
}

void Plugin_WallPaper::slotSetCenteredMaxpect()
{
   return setWallpaper(CENTER_MAXPECT);
}

void Plugin_WallPaper::slotSetTiledMaxpect()
{
   return setWallpaper(TILED_MAXPECT);
}

void Plugin_WallPaper::slotSetScaled()
{
   return setWallpaper(SCALED);
}

void Plugin_WallPaper::slotSetCenteredAutoFit()
{
   return setWallpaper(CENTERED_AUTOFIT);
}

void Plugin_WallPaper::slotSetScaleAndCrop()
{
   return setWallpaper(SCALE_AND_CROP);
}

void Plugin_WallPaper::setWallpaper(int layout)
{
   if (layout>SCALE_AND_CROP || layout < CENTER)
      return;

   KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

   if ( !interface )
   {
       kError() << "Kipi interface is null!";
       return;
   }

   KIPI::ImageCollection images = interface->currentSelection();

   if (!images.isValid() ) return;

   KUrl url=images.images()[0];
   QString path;
   if (url.isLocalFile())
   {
      path=url.path();
   }
   else
   {
      // PENDING We need a way to get a parent widget
      // Sun, 06 Jun 2004 - Aurelien

      KMessageBox::information( kapp->activeWindow(), i18n(
         "<p>You selected a remote image. It needs to be saved to your local disk to be used as a wallpaper."
         "</p><p>You will now be asked where to save the image.</p>"));
      path = KFileDialog::getSaveFileName(url.fileName(), QString::null, kapp->activeWindow());

      if (path.isNull()) return;
      KIO::NetAccess::download(url, path, 0L);
   }
   //TODO verify when we change it with plasma
   OrgKdeKdesktopBackgroundInterface desktopInterface("org.kde.kdesktop", "/Background", QDBusConnection::sessionBus());
   QDBusReply<void> reply = desktopInterface.setWallpaper(path,layout);
   if(!reply.isValid())
      KMessageBox::information(0L,i18n("Change Background"),i18n("Background cannot be changed."));
}

KIPI::Category  Plugin_WallPaper::category( KAction* action ) const
{
    if ( action == m_actionBackground )
       return KIPI::IMAGESPLUGIN;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}
