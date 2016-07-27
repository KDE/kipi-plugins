/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
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

#include "plugin_imageshack.h"

// Qt includes

#include <QPointer>
#include <QAction>
#include <QApplication>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "imageshack.h"
#include "imageshackwindow.h"
#include "kipiplugins_debug.h"

namespace KIPIImageshackPlugin
{

K_PLUGIN_FACTORY(Factory, registerPlugin<Plugin_Imageshack>();)

class Plugin_Imageshack::Private
{
public:

    Private() :
        actionExport(0),
        imageshack(0)
    {
    }

    QAction*    actionExport;
    Imageshack* imageshack;
};

Plugin_Imageshack::Plugin_Imageshack(QObject* const parent, const QVariantList&)
    : Plugin(parent, "Imageshack"),
      d(new Private())
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Imageshack plugin loaded";

    d->imageshack = new Imageshack();

    setUiBaseName("kipiplugin_imageshackui.rc");
    setupXML();
}

Plugin_Imageshack::~Plugin_Imageshack()
{
    delete d->imageshack;
    delete d;
}

void Plugin_Imageshack::setup(QWidget* const widget)
{
    Plugin::setup(widget);
    setupActions();

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    d->actionExport->setEnabled(true);
}

void Plugin_Imageshack::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->actionExport = new QAction(this);
    d->actionExport->setText(i18n("Export to &Imageshack..."));
    d->actionExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-imageshack")));
    actionCollection()->setDefaultShortcut(d->actionExport, Qt::ALT + Qt::SHIFT + Qt::Key_M);
    d->actionExport->setEnabled(false);

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(QString::fromLatin1("imageshackexport"), d->actionExport);
}

void Plugin_Imageshack::slotExport()
{
    QPointer<ImageshackWindow> dlg;

    dlg = new ImageshackWindow(QApplication::activeWindow(), d->imageshack);
    dlg->exec();

    delete dlg;
}

} // namespace KIPIImageshackPlugin

#include "plugin_imageshack.moc"
