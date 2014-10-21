/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

class Plugin_TimeAdjust::Private
{
public:

    Private()
    {
        actionTimeAjust = 0;
        dialog          = 0;
    }

    KAction*          actionTimeAjust;
    TimeAdjustDialog* dialog;
};

Plugin_TimeAdjust::Plugin_TimeAdjust(QObject* const parent, const QVariantList&)
    : Plugin(TimeAdjustFactory::componentData(), parent, "TimeAdjust"),
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_TimeAdjust plugin loaded";

    setUiBaseName("kipiplugin_timeadjustui.rc");
    setupXML();
}

Plugin_TimeAdjust::~Plugin_TimeAdjust()
{
    delete d;
}

void Plugin_TimeAdjust::setup(QWidget* const widget)
{
    Plugin::setup(widget);
    setupActions();

    if (!interface())
    {
       kError() << "Kipi interface is null!";
       return;
    }

    ImageCollection selection = interface()->currentSelection();
    d->actionTimeAjust->setEnabled(selection.isValid() && !selection.images().isEmpty());

    connect(interface(), SIGNAL(selectionChanged(bool)),
            d->actionTimeAjust, SLOT(setEnabled(bool)));
}

void Plugin_TimeAdjust::setupActions()
{
    setDefaultCategory(ImagesPlugin);

    d->actionTimeAjust = new KAction(this);
    d->actionTimeAjust->setText(i18n("Adjust Time && Date..."));
    d->actionTimeAjust->setIcon(KIcon("kipi-timeadjust"));
    d->actionTimeAjust->setEnabled(false);

    connect(d->actionTimeAjust, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("timeadjust", d->actionTimeAjust);
}

void Plugin_TimeAdjust::slotActivate()
{
    ImageCollection images = interface()->currentSelection();

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

    if (!interface()->hasFeature(KIPI::ImagesHasTime))
        d->dialog->disableApplTimestamp();

    d->dialog->addItems(images.images());
}

}  // namespace KIPITimeAdjustPlugin
