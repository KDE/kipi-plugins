/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2003-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "plugin_sendimages.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <klibloader.h>
#include <klocale.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "sendimages.h"
#include "sendimagesdialog.h"

namespace KIPISendimagesPlugin
{

K_PLUGIN_FACTORY( SendImagesFactory, registerPlugin<Plugin_SendImages>(); )
K_EXPORT_PLUGIN ( SendImagesFactory("kipiplugin_sendimages") )

class Plugin_SendImages::Private
{
public:

    Private()
    {
        dialog              = 0;
        action_sendimages   = 0;
        sendImagesOperation = 0;
    }

    KAction*          action_sendimages;

    SendImagesDialog* dialog;

    SendImages*       sendImagesOperation;
};

Plugin_SendImages::Plugin_SendImages(QObject* const parent, const QVariantList&)
    : Plugin(SendImagesFactory::componentData(), parent, "SendImages"), 
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_SendImages plugin loaded";

    setUiBaseName("kipiplugin_sendimagesui.rc");
    setupXML();
}

Plugin_SendImages::~Plugin_SendImages()
{
    delete d;
}

void Plugin_SendImages::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    setupActions();

    Interface* const iface = interface();

    if (!iface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    ImageCollection selection = iface->currentSelection();
    d->action_sendimages->setEnabled(selection.isValid() && !selection.images().isEmpty() );

    connect(iface, SIGNAL(selectionChanged(bool)),
            d->action_sendimages, SLOT(setEnabled(bool)));
}

void Plugin_SendImages::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->action_sendimages = new KAction(this);
    d->action_sendimages->setText(i18n("Email Images..."));
    d->action_sendimages->setIcon(KIcon("mail-send"));

    connect(d->action_sendimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("sendimages", d->action_sendimages);
}

void Plugin_SendImages::slotActivate()
{
    Interface* const iface = interface();

    if (!iface)
    {
       kError() << "Kipi interface is null!";
       return;
    }

    ImageCollection images = iface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    delete d->dialog;

    d->dialog = new SendImagesDialog(kapp->activeWindow(), images.images());
    d->dialog->show();

    connect(d->dialog, SIGNAL(accepted()),
            this, SLOT(slotPrepareEmail()));
}

void Plugin_SendImages::slotPrepareEmail()
{
    Interface* const interface = dynamic_cast<Interface*>(parent());

    if (!interface)
    {
       kError() << "Kipi interface is null!";
       return;
    }

    EmailSettings settings = d->dialog->emailSettings();
    d->sendImagesOperation = new SendImages(settings, this);
    d->sendImagesOperation->firstStage();
}

} // namespace KIPISendimagesPlugin
