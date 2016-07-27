/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2003-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QApplication>
#include <QAction>

// KDE includes

#include <klocalizedstring.h>
#include <kpluginfactory.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>

// Local includes

#include "kputil.h"
#include "sendimages.h"
#include "sendimagesdialog.h"
#include "kipiplugins_debug.h"

namespace KIPISendimagesPlugin
{

K_PLUGIN_FACTORY(SendImagesFactory, registerPlugin<Plugin_SendImages>();)

class Plugin_SendImages::Private
{
public:

    Private()
    {
        dialog              = 0;
        action_sendimages   = 0;
        sendImagesOperation = 0;
    }

    QAction *          action_sendimages;

    SendImagesDialog* dialog;

    SendImages*       sendImagesOperation;
};

Plugin_SendImages::Plugin_SendImages(QObject* const parent, const QVariantList&)
    : Plugin(parent, "SendImages"),
      d(new Private)
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_SendImages plugin loaded";

    setUiBaseName("kipiplugin_sendimagesui.rc");
    setupXML();
}

Plugin_SendImages::~Plugin_SendImages()
{
    delete d->sendImagesOperation;
    delete d->dialog;
    delete d;

    removeTemporaryDir("sendimages");
}

void Plugin_SendImages::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    setupActions();

    Interface* const iface = interface();

    if (!iface)
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
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

    d->action_sendimages = new QAction(this);
    d->action_sendimages->setText(i18n("Email Images..."));
    d->action_sendimages->setIcon(QIcon::fromTheme(QLatin1String("mail-send")));

    connect(d->action_sendimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(QLatin1String("sendimages"), d->action_sendimages);
}

void Plugin_SendImages::slotActivate()
{
    Interface* const iface = interface();

    if (!iface)
    {
       qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
       return;
    }

    ImageCollection images = iface->currentSelection();

    if (!images.isValid() || images.images().isEmpty())
        return;

    delete d->dialog;

    d->dialog = new SendImagesDialog(QApplication::activeWindow(), images.images());
    d->dialog->show();

    connect(d->dialog, SIGNAL(accepted()),
            this, SLOT(slotPrepareEmail()));
}

void Plugin_SendImages::slotPrepareEmail()
{
    Interface* const interface = dynamic_cast<Interface*>(parent());

    if (!interface)
    {
       qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
       return;
    }

    delete d->sendImagesOperation;

    EmailSettings settings = d->dialog->emailSettings();
    d->sendImagesOperation = new SendImages(settings, this);
    d->sendImagesOperation->firstStage();
}

} // namespace KIPISendimagesPlugin

#include "plugin_sendimages.moc"
