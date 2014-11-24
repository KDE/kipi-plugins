/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-05-16
 * Description : a plugin to acquire image using flat scanner.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "plugin_acquireimages.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

// LibKSane includes

#include <libksane/ksane.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes

#include "aboutdata.h"
#include "scandialog.h"

namespace KIPIAcquireImagesPlugin
{

K_PLUGIN_FACTORY(AcquireImagesFactory, registerPlugin<Plugin_AcquireImages>();)
K_EXPORT_PLUGIN(AcquireImagesFactory("kipiplugin_acquireimages"))

Plugin_AcquireImages::Plugin_AcquireImages(QObject* const parent, const QVariantList&)
    : Plugin(AcquireImagesFactory::componentData(), parent, "AcquireImages")
{
    m_action_scanimages = 0;
    m_parentWidget      = 0;
    m_saneWidget        = 0;
    m_scanDlg           = 0;
    kDebug(AREA_CODE_LOADING) << "Plugin_AcquireImages plugin loaded";

    setUiBaseName("kipiplugin_acquireimagesui.rc");
    setupXML();
}

Plugin_AcquireImages::~Plugin_AcquireImages()
{
}

void Plugin_AcquireImages::setup(QWidget* const widget)
{
    m_parentWidget = widget;
    Plugin::setup(m_parentWidget);
    setupActions();

    if (!interface())
    {
        kError() << "KIPI interface is null!";
        return;
    }

    m_action_scanimages->setEnabled(true);
}

void Plugin_AcquireImages::setupActions()
{
    setDefaultCategory(ImportPlugin);

    m_action_scanimages = new KAction(this);
    m_action_scanimages->setText(i18n("Import from Scanner..."));
    m_action_scanimages->setIcon(KIcon("scanner"));
    m_action_scanimages->setEnabled(false);

    connect(m_action_scanimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("acquireimages", m_action_scanimages);
}

void Plugin_AcquireImages::slotActivate()
{
    if (!m_saneWidget)
    {
        m_saneWidget = new KSaneIface::KSaneWidget(0);
    }

    if (m_saneWidget)
    {
        QString dev = m_saneWidget->selectDevice(0);

        if (dev.isEmpty())
        {
            return;
        }

        if (!m_saneWidget->openDevice(dev))
        {
            // could not open a scanner
            KMessageBox::sorry(0, i18n("Cannot open scanner device."));
            return;
        }
    }

    if (!m_scanDlg)
    {
        m_scanDlg = new ScanDialog(m_saneWidget, kapp->activeWindow(), new ScanDialogAboutData);
    }
    else
    {
        if (m_scanDlg->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_scanDlg->winId());
        }

        KWindowSystem::activateWindow(m_scanDlg->winId());
    }

    m_scanDlg->show();
}

}  // namespace KIPIAcquireImagesPlugin
