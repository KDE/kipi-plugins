/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : a kipi plugin to convert Raw file to DNG
 *
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_dngconverter.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "aboutdata.h"
#include "batchdialog.h"

namespace KIPIDNGConverterPlugin
{

K_PLUGIN_FACTORY( RawConverterFactory, registerPlugin<Plugin_DNGConverter>(); )
K_EXPORT_PLUGIN ( RawConverterFactory("kipiplugin_dngconverter") )

Plugin_DNGConverter::Plugin_DNGConverter(QObject* const parent, const QVariantList&)
    : Plugin( RawConverterFactory::componentData(), parent, "DNGConverter")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_DNGConverter plugin loaded" ;
}

void Plugin_DNGConverter::setup( QWidget* widget )
{
    m_batchDlg = 0;

    Plugin::setup( widget );

    m_action = actionCollection()->addAction("dngconverter");
    m_action->setText(i18n("DNG Converter..."));
    m_action->setIcon(KIcon("dngconverter"));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_action);

    Interface* interface = dynamic_cast<Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action, SLOT(setEnabled(bool)));
}

Plugin_DNGConverter::~Plugin_DNGConverter()
{
}

void Plugin_DNGConverter::slotActivate()
{
    Interface* interface = dynamic_cast<Interface*>( parent() );
    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    ImageCollection images = interface->currentSelection();

    if (!images.isValid())
        return;

    if (!m_batchDlg)
    {
        m_batchDlg = new BatchDialog(new DNGConverterAboutData);
    }
    else
    {
        if (m_batchDlg->isMinimized())
            KWindowSystem::unminimizeWindow(m_batchDlg->winId());

        KWindowSystem::activateWindow(m_batchDlg->winId());
    }

    m_batchDlg->show();
    m_batchDlg->addItems(images.images());
}

Category Plugin_DNGConverter::category( KAction* action ) const
{
    if (action == m_action)
       return BatchPlugin;

    kWarning() << "Unrecognized action for plugin category identification" ;
    return BatchPlugin; // no warning from compiler, please
}

} // namespace KIPIDNGConverterPlugin
