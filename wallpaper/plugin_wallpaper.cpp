/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-01-01
 * Description : Wall Paper kipi-plugin
 *
 * Copyright (C) 2004      by Gregory Kokanosky <gregory dot kokanosky at free.fr>
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Varun Herale <varun dot herale at gmail dot com>
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

#include "plugin_wallpaper.moc"

// QT includes

#include <QDBusInterface>
#include <QDBusReply>

// KDE includes

#include <klocale.h>
#include <kactioncollection.h>
#include <kmenu.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>
#include <libkipi/pluginloader.h>

namespace KIPIWallPaperPlugin
{

K_PLUGIN_FACTORY( WallPaperFactory, registerPlugin<Plugin_WallPaper>(); )
K_EXPORT_PLUGIN ( WallPaperFactory("kipiplugin_wallpaper") )

class Plugin_WallPaper::Private
{

public:

    Private()
    {
        actionBackground = 0;
        interface        = 0;
        widget           = 0;
    }

    KAction*     actionBackground;
    Interface* interface;
    QWidget*   widget;
};

Plugin_WallPaper::Plugin_WallPaper(QObject* const parent, const QVariantList&)
    : Plugin(WallPaperFactory::componentData(), parent, "WallPaper"),
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_WallPaper plugin loaded";

    setUiBaseName("kipiplugin_wallpaperui.rc");
    setupXML();
}

Plugin_WallPaper::~Plugin_WallPaper()
{
    delete d->actionBackground;
    delete d;
}

void Plugin_WallPaper::setup(QWidget* const widget)
{
    d->widget = widget;

    Plugin::setup(widget);
    setupActions();

    d->interface = interface();
    if ( !d->interface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    ImageCollection selection = d->interface->currentSelection();

    if (d->actionBackground)
    {
        d->actionBackground->setEnabled( selection.isValid() && !selection.images().isEmpty() );

        connect(d->interface, SIGNAL(selectionChanged(bool)),
                d->actionBackground, SLOT(setEnabled(bool)));
    }
 }

void Plugin_WallPaper::setupActions()
{
    setDefaultCategory(ImagesPlugin);

    QStringList disabledActions = PluginLoader::instance()->disabledPluginActions();
    if (disabledActions.contains("images2desktop")
        || disabledActions.contains("images2desktop_setwallpaper"))
        return;

    d->actionBackground = actionCollection()->addAction("images2desktop");
    d->actionBackground->setIcon(KIcon("image-jpeg"));
    d->actionBackground->setText(i18n("&Set as Background"));
    d->actionBackground->setEnabled(false);

    KMenu* menu = new KMenu(d->widget);
    d->actionBackground->setMenu(menu);

    KAction* wallpaper = new KAction(this);
    wallpaper->setText(i18n("Set as wallpaper"));

    connect(wallpaper, SIGNAL(triggered(bool)),
            this, SLOT(slotSetWallpaper()));

    menu->addAction(wallpaper);

    addAction("images2desktop_setwallpaper", wallpaper);
}

void Plugin_WallPaper::slotSetWallpaper()
{
   ImageCollection images = d->interface->currentSelection();

   if ( !images.isValid() )
       return;

   KUrl url                      = images.images()[0];
   QString path                  = url.path();
   QDBusInterface* dbusInterface = new QDBusInterface("org.kde.plasma-desktop", "/App", "local.PlasmaApp");
   QDBusReply<void> reply        = dbusInterface->call("setWallpaperImage", path);

   if ( !reply.isValid() )
       KMessageBox::information(0, i18n("Background cannot be changed. You do not have the correct version "
                                        "of kde-workspace."), i18n("Change Background"));

   delete dbusInterface;
}

} // namespace KIPIWallPaperPlugin
