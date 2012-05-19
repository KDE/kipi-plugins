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

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/plugin.h>

// Local includes

#include "imageshack.h"
#include "imageshackwindow.h"

namespace KIPIImageshackExportPlugin
{

K_PLUGIN_FACTORY(Factory, registerPlugin<Plugin_ImageshackExport>();)
K_EXPORT_PLUGIN(Factory("kipiplugin_imageshackexport"))

Plugin_ImageshackExport::Plugin_ImageshackExport(QObject* const parent, const QVariantList&)
    : Plugin(Factory::componentData(), parent, "ImageshackExport"),
      m_action(0), m_pImageshack(0)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_ImageshackExport plugin loaded";
}

void Plugin_ImageshackExport::setup(QWidget* widget)
{
    KIconLoader::global()->addAppDir("kipiplugin_imageshackexport");

    m_pImageshack = new Imageshack();

    Plugin::setup(widget);

    Interface* interface = dynamic_cast<Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_action = actionCollection()->addAction("imageshackexport");
    m_action->setText(i18n("Export to &Imageshack..."));
    m_action->setIcon(KIcon("imageshack"));
    m_action->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_M));
    m_action->setEnabled(true);

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(m_action);
}

Plugin_ImageshackExport::~Plugin_ImageshackExport()
{
    delete m_pImageshack;
}

void Plugin_ImageshackExport::slotExport()
{
    kDebug() << "Loading Imageshack Export Window";

    QPointer<ImageshackWindow> dlg;

    dlg = new ImageshackWindow(kapp->activeWindow(), m_pImageshack);
    dlg->exec();

    delete dlg;
}

Category Plugin_ImageshackExport::category(KAction* action) const
{
    if (action == m_action)
        return ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ExportPlugin;
}

} // namespace KIPIImageshackExportPlugin
