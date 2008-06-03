/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes.

#include <kiconloader.h>
#include <klocale.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// LibKSane includes.

#include <libksane/ksane.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes.

#include "scandialog.h"
#include "plugin_acquireimages.h"
#include "plugin_acquireimages.moc"

K_PLUGIN_FACTORY( AcquireImagesFactory, registerPlugin<Plugin_AcquireImages>(); )
K_EXPORT_PLUGIN ( AcquireImagesFactory("kipiplugin_acquireimages") )

Plugin_AcquireImages::Plugin_AcquireImages(QObject *parent, const QVariantList&)
                    : KIPI::Plugin( AcquireImagesFactory::componentData(), parent, "AcquireImages")
{
    kDebug( 51001 ) << "Plugin_AcquireImages plugin loaded";
}

void Plugin_AcquireImages::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_action_scanimages = new KAction(KIcon("scanner"), i18n("Scan Images..."), actionCollection());
    m_action_scanimages->setObjectName("scan_images");
    connect(m_action_scanimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));
    addAction(m_action_scanimages);

    m_interface = dynamic_cast< KIPI::Interface* >(parent());
    if (!m_interface)
    {
       kError( 51000 ) << "Kipi interface is null!";
       return;
    }
}

Plugin_AcquireImages::~Plugin_AcquireImages()
{
}

void Plugin_AcquireImages::slotActivate()
{
    KSaneIface::KSaneWidget *saneWidget = new KSaneIface::KSaneWidget(0);

    QString dev = saneWidget->selectDevice(0);
    if (dev.isEmpty())
        return;

    if (!saneWidget->openDevice(dev))
    {
        // could not open a scanner
        KMessageBox::sorry(0, i18n("Cannot open scanner device."));
        return;
    }

    KIPIAcquireImagesPlugin::ScanDialog dlg(m_interface, saneWidget, kapp->activeWindow());
    dlg.exec();
}

KIPI::Category Plugin_AcquireImages::category( KAction* action ) const
{
    if ( action == m_action_scanimages )
       return KIPI::ImportPlugin;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification";
    return KIPI::ImportPlugin; // no warning from compiler, please
}
