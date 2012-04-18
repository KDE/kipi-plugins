/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_timeadjust.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kwindowsystem.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <klocale.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes

#include "timeadjustdialog.h"

namespace KIPITimeAdjustPlugin
{

K_PLUGIN_FACTORY( TimeAdjustFactory, registerPlugin<Plugin_TimeAdjust>(); )
K_EXPORT_PLUGIN ( TimeAdjustFactory("kipiplugin_timeadjust") )

class Plugin_TimeAdjust::Plugin_TimeAdjustPriv
{
public:

    Plugin_TimeAdjustPriv()
    {
        actionTimeAjust = 0;
        interface       = 0;
        dialog          = 0;
    }

    KAction*          actionTimeAjust;
    Interface*        interface;
    TimeAdjustDialog* dialog;
};

Plugin_TimeAdjust::Plugin_TimeAdjust(QObject* const parent, const QVariantList&)
    : Plugin(TimeAdjustFactory::componentData(), parent, "TimeAdjust"),
      d(new Plugin_TimeAdjustPriv)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_TimeAdjust plugin loaded";
}

Plugin_TimeAdjust::~Plugin_TimeAdjust()
{
    delete d;
}

void Plugin_TimeAdjust::setup(QWidget* widget)
{
    Plugin::setup(widget);

    d->actionTimeAjust = actionCollection()->addAction("timeadjust");
    d->actionTimeAjust->setText(i18n("Adjust Time && Date..."));
    d->actionTimeAjust->setIcon(KIcon("timeadjust"));

    connect(d->actionTimeAjust, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(d->actionTimeAjust);

    d->interface = dynamic_cast<Interface*>(parent());
    if (!d->interface)
    {
       kError() << "Kipi interface is null!";
       return;
    }

    ImageCollection selection = d->interface->currentSelection();
    d->actionTimeAjust->setEnabled(selection.isValid() && !selection.images().isEmpty());

    connect(d->interface, SIGNAL(selectionChanged(bool)),
            d->actionTimeAjust, SLOT(setEnabled(bool)));
}

void Plugin_TimeAdjust::slotActivate()
{
    ImageCollection images = d->interface->currentSelection();

    if (!images.isValid() || images.images().isEmpty())
        return;

    if (!d->dialog)
    {
        d->dialog = new TimeAdjustDialog();
    }
    else
    {
        if (d->dialog->isMinimized())
            KWindowSystem::unminimizeWindow(d->dialog->winId());

        KWindowSystem::activateWindow(d->dialog->winId());
    }

    d->dialog->show();
    d->dialog->addItems(images.images());
}

Category Plugin_TimeAdjust::category(KAction* action) const
{
    if ( action == d->actionTimeAjust )
       return ImagesPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ImagesPlugin; // no warning from compiler, please
}

}  // namespace KIPITimeAdjustPlugin
