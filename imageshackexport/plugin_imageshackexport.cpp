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

#include "plugin_imageshackexport.moc"

// Qt includes

#include <QPointer>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kxmlguiclient.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "imageshack.h"
#include "imageshackwindow.h"

namespace KIPIImageshackExportPlugin
{

K_PLUGIN_FACTORY(Factory, registerPlugin<Plugin_ImageshackExport>();)
K_EXPORT_PLUGIN(Factory("kipiplugin_imageshackexport"))

class Plugin_ImageshackExport::Private
{
public:

    Private() :
        actionExport(0),
        iface(0),
        imageshack(0)
    {
    }

    KAction*    actionExport;
    Interface*  iface;
    Imageshack* imageshack;
};

Plugin_ImageshackExport::Plugin_ImageshackExport(QObject* const parent, const QVariantList&)
    : Plugin(Factory::componentData(), parent, "ImageshackExport"),
      d(new Private())
{
    kDebug() << "Plugin_ImageshackExport plugin loaded";

    KIconLoader::global()->addAppDir("kipiplugin_imageshackexport");

    d->imageshack = new Imageshack();
    d->iface = dynamic_cast<Interface*>(parent);
    if (!d->iface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    setupActions();
    setupXML();
}

Plugin_ImageshackExport::~Plugin_ImageshackExport()
{
    delete d->imageshack;
    delete d;
}

void Plugin_ImageshackExport::setup(QWidget* const widget)
{
    Plugin::setup(widget);
}

void Plugin_ImageshackExport::setupActions()
{
    d->actionExport = actionCollection()->addAction("imageshackexport");
    d->actionExport->setText(i18n("Export to &Imageshack..."));
    d->actionExport->setIcon(KIcon("imageshack"));
    d->actionExport->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_M));
    d->actionExport->setEnabled(true);

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(d->actionExport);

    setUiBaseName("kipiplugin_imageshackexportui.rc");
}

void Plugin_ImageshackExport::setupXML()
{
    KXMLGUIClient* host = dynamic_cast<KXMLGUIClient*>(d->iface->parent());
    mergeXMLFile(host);
}

void Plugin_ImageshackExport::slotExport()
{
    QPointer<ImageshackWindow> dlg;

    dlg = new ImageshackWindow(kapp->activeWindow(), d->imageshack);
    dlg->exec();

    delete dlg;
}

Category Plugin_ImageshackExport::category(KAction* const action) const
{
    if (action == d->actionExport)
        return ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ExportPlugin;
}

} // namespace KIPIImageshackExportPlugin
