/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-01-31
 * Description : a kipi plugin to export images as video slide show
 *
 * Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#include "plugin_videoslideshow.moc"

// C++ includes

#include <cstdlib>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "exportdialog.h"

namespace KIPIVideoSlideShowPlugin
{

K_PLUGIN_FACTORY( VideoSlideShowFactory, registerPlugin<Plugin_VideoSlideShow>(); )
K_EXPORT_PLUGIN ( VideoSlideShowFactory("kipiplugin_videoslideshow") )

class Plugin_VideoSlideShow::Private
{
public:

    Private()
    {
        exportAction = 0;
        exportDlg    = 0;
    }
    KAction*      exportAction;
    ExportDialog* exportDlg;
};

Plugin_VideoSlideShow::Plugin_VideoSlideShow(QObject* const parent, const QVariantList&)
    : Plugin(VideoSlideShowFactory::componentData(), parent, "VideoSlideShow"),
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_VideoSlideShow plugin loaded";
}

Plugin_VideoSlideShow::~Plugin_VideoSlideShow()
{
    delete d;
}

void Plugin_VideoSlideShow::setup(QWidget* const widget)
{
    d->exportDlg = 0;

    Plugin::setup( widget );

    d->exportAction = actionCollection()->addAction("video_slide_show");
    d->exportAction->setText(i18n("Export to Video Slide Show..."));

    connect(d->exportAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(d->exportAction);

    Interface* interface = dynamic_cast<Interface*>( parent() );
    if ( !interface )
    {
           kError() << "Kipi interface is null!";
           return;
    }

    connect(interface, SIGNAL(selectionChanged(bool)),
            d->exportAction, SLOT(setEnabled(bool)));

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            d->exportAction, SLOT(setEnabled(bool)));
}

void Plugin_VideoSlideShow::slotExport()
{
    Interface* interface = dynamic_cast<Interface*>( parent() );
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    ImageCollection images = interface->currentSelection();

    if (!images.isValid())
        return;

    if ( images.images().isEmpty() )
        return;

    if (!d->exportDlg)
    {
        d->exportDlg = new ExportDialog(images);
    }
    else
    {
        if (d->exportDlg->isMinimized())
            KWindowSystem::unminimizeWindow(d->exportDlg->winId());

        KWindowSystem::activateWindow(d->exportDlg->winId());
        d->exportDlg->setImages(images);
    }

    d->exportDlg->show();
}

Category Plugin_VideoSlideShow::category(KAction* const action) const
{
    if ( action == d->exportAction )
       return ToolsPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ToolsPlugin; // no warning from compiler, please
}

} // namespace KIPIVideoSlideShowPlugin
