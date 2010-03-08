/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export or import image to imgur.com
 *
 * Copyright (C) 2010 by Marius Orcisk <marius at habarnam dot ro>
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

#include "plugin_imgurexport.h"

// KDE includes
#include <KDebug>
#include <KConfig>
#include <KApplication>
#include <KAction>
#include <KActionCollection>
#include <KGenericFactory>
#include <KLibLoader>
#include <KStandardDirs>
#include <KLocale>
#include <KPluginFactory>
#include <KPluginLoader>


// LibKipi includes
#include <libkipi/interface.h>

using namespace KIPIImgurExportPlugin;

K_PLUGIN_FACTORY( ImgurExportFactory, registerPlugin<Plugin_ImgurExport>(); )
K_EXPORT_PLUGIN ( ImgurExportFactory("kipiplugin_imgurexport") )

Plugin_ImgurExport::Plugin_ImgurExport(QObject *parent, const QVariantList &args)
                      : KIPI::Plugin(ImgurExportFactory::componentData(), parent, "ImgurExport")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_ImgurExport plugin loaded";
}

void Plugin_ImgurExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_imgurexport");

    m_actionExport = actionCollection()->addAction("ImgurExport");
    m_actionExport->setText(i18n("Export to &Imgur..."));
    m_actionExport->setIcon(KIcon("imgur"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_I));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_actionExport);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        m_actionExport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
}

void Plugin_ImgurExport::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KStandardDirs dir;
    QString Tmp = dir.saveLocation("tmp", "kipi-imgurexportplugin-" + QString::number(getpid()) + '/');

    kDebug() << "We have activated imgur exporter!";
}

KIPI::Category Plugin_ImgurExport::category( KAction* action ) const
{
    return KIPI::ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ExportPlugin;
}
