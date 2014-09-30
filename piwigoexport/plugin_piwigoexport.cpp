/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2014-09-30
 * Description : a plugin to export to a remote Piwigo server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010-2014 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#include "plugin_piwigoexport.moc"

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

#include "piwigos.h"
#include "piwigoconfig.h"
#include "piwigowindow.h"

namespace KIPIPiwigoExportPlugin
{

K_PLUGIN_FACTORY(Factory, registerPlugin<Plugin_PiwigoExport>();)
K_EXPORT_PLUGIN(Factory("kipiplugin_piwigoexport"))

Plugin_PiwigoExport::Plugin_PiwigoExport(QObject* const parent, const QVariantList&)
    : Plugin(Factory::componentData(), parent, "PiwigoExport"),
      m_action(0), m_pPiwigo(0)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_PiwigoExport plugin loaded";

    setUiBaseName("kipiplugin_piwigoexportui.rc");
    setupXML();
}

Plugin_PiwigoExport::~Plugin_PiwigoExport()
{
    delete m_pPiwigo;
}

void Plugin_PiwigoExport::setup(QWidget* const widget)
{
    KIconLoader::global()->addAppDir("kipiplugin_piwigoexport");

    m_pPiwigo = new Piwigo();

    Plugin::setup(widget);

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    setupActions();
}

void Plugin_PiwigoExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_action = new KAction(this);
    m_action->setText(i18n("Export to &Piwigo..."));
    m_action->setIcon(KIcon("kipi-piwigo"));
    m_action->setEnabled(true);

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotSync()));

    addAction("piwigoexport", m_action);
}

// this slot uses PiwigoWindow Class
void Plugin_PiwigoExport::slotSync()
{
    QPointer<PiwigoEdit>   configDlg;
    QPointer<PiwigoWindow> dlg;

    KConfig config("kipirc");

    if (!config.hasGroup("Piwigo Settings") )
    {
        configDlg = new PiwigoEdit(kapp->activeWindow(), m_pPiwigo, i18n("Edit Piwigo Data") );
        configDlg->exec();
    }

    dlg = new PiwigoWindow(kapp->activeWindow(), m_pPiwigo);
    dlg->exec();

    delete configDlg;
    delete dlg;
}

} // namespace KIPIPiwigoExportPlugin
