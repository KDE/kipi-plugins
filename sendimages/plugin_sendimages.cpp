/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2003-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_sendimages.h"
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

// Local includes

#include "sendimages.h"
#include "sendimagesdialog.h"

K_PLUGIN_FACTORY( SendImagesFactory, registerPlugin<Plugin_SendImages>(); )
K_EXPORT_PLUGIN ( SendImagesFactory("kipiplugin_sendimages") )

using namespace KIPISendimagesPlugin;

class Plugin_SendImagesPriv
{
public:

    Plugin_SendImagesPriv()
    {
        dialog              = 0;
        action_sendimages   = 0;
        sendImagesOperation = 0;
    }

    KAction          *action_sendimages;

    SendImagesDialog *dialog;

    SendImages       *sendImagesOperation;
};

Plugin_SendImages::Plugin_SendImages(QObject *parent, const QVariantList&)
                 : KIPI::Plugin(SendImagesFactory::componentData(), parent, "SendImages"), 
                   d(new Plugin_SendImagesPriv)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_SendImages plugin loaded";
}

Plugin_SendImages::~Plugin_SendImages()
{
    delete d;
}

void Plugin_SendImages::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    d->action_sendimages = actionCollection()->addAction("sendimages");
    d->action_sendimages->setText(i18n("Email Images..."));
    d->action_sendimages->setIcon(KIcon("mail-send"));

    connect(d->action_sendimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(d->action_sendimages);

    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
    if ( !interface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KIPI::ImageCollection selection = interface->currentSelection();
    d->action_sendimages->setEnabled(selection.isValid() && !selection.images().isEmpty() );

    connect(interface, SIGNAL(selectionChanged(bool)),
            d->action_sendimages, SLOT(setEnabled(bool)));
}

void Plugin_SendImages::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
    if ( !interface )
    {
       kError() << "Kipi interface is null!";
       return;
    }

    KIPI::ImageCollection images = interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (d->dialog) delete d->dialog;

    d->dialog = new SendImagesDialog(kapp->activeWindow(), interface, images.images());
    d->dialog->show();

    connect(d->dialog, SIGNAL(okClicked()),
            this, SLOT(slotPrepareEmail()));
}

void Plugin_SendImages::slotPrepareEmail()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
    if ( !interface )
    {
       kError() << "Kipi interface is null!";
       return;
    }

    EmailSettingsContainer settings = d->dialog->emailSettings();
    d->sendImagesOperation          = new SendImages(settings, this, interface);
    d->sendImagesOperation->sendImages();
}

KIPI::Category Plugin_SendImages::category( KAction* action ) const
{
    if ( action == d->action_sendimages )
       return KIPI::ImagesPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ImagesPlugin; // no warning from compiler, please
}
