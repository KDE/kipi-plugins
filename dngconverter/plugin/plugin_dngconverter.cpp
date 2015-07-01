/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : a kipi plugin to convert Raw file to DNG
 *
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_dngconverter.h"

// Qt includes

#include <QApplication>
#include <QAction>

// KDE includes

#include <kactioncollection.h>
#include <kconfig.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>

// Local includes

#include "aboutdata.h"
#include "batchdialog.h"
#include "kipiplugins_debug.h"

namespace KIPIDNGConverterPlugin
{

K_PLUGIN_FACTORY( RawConverterFactory, registerPlugin<Plugin_DNGConverter>(); )

Plugin_DNGConverter::Plugin_DNGConverter(QObject* const parent, const QVariantList&)
    : Plugin(parent, "DNGConverter")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_DNGConverter plugin loaded" ;

    setUiBaseName("kipiplugin_dngconverterui.rc");
    setupXML();
}

Plugin_DNGConverter::~Plugin_DNGConverter()
{
}

void Plugin_DNGConverter::setup(QWidget* const widget)
{
    m_batchDlg = 0;

    Plugin::setup(widget);

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    setupActions();

    connect(interface(), SIGNAL(currentAlbumChanged(bool)),
            m_action, SLOT(setEnabled(bool)));
}

void Plugin_DNGConverter::setupActions()
{
    setDefaultCategory(BatchPlugin);

    m_action = new QAction(this);
    m_action->setText(i18n("DNG Converter..."));
    m_action->setIcon(QIcon::fromTheme("kipi-dngconverter"));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("dngconverter", m_action);
}

void Plugin_DNGConverter::slotActivate()
{
    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!" ;
        return;
    }

    ImageCollection images = interface()->currentSelection();

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

} // namespace KIPIDNGConverterPlugin

#include "plugin_dngconverter.moc"
