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

#include "plugin_acquireimages.h"
#include "plugin_acquireimages.moc"

// KDE includes.

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

#ifdef WIN32

// Twain interface includes.
#include <qtwain.h>

#else /*  WIN32 */

// LibKSane includes.
#include <libksane/ksane.h>

// Local includes.
#include "scandialog.h"

#endif /*  WIN32 */

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

K_PLUGIN_FACTORY( AcquireImagesFactory, registerPlugin<Plugin_AcquireImages>(); )
K_EXPORT_PLUGIN ( AcquireImagesFactory("kipiplugin_acquireimages") )

Plugin_AcquireImages::Plugin_AcquireImages(QObject *parent, const QVariantList&)
                    : KIPI::Plugin( AcquireImagesFactory::componentData(), parent, "AcquireImages")
{
    m_parentWidget = 0;
    kDebug( 51001 ) << "Plugin_AcquireImages plugin loaded";
}

void Plugin_AcquireImages::setup(QWidget* widget)
{
    m_parentWidget = widget;

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
#ifdef WIN32

    QTwain *twIface = new QTwain(m_parentWidget);

    connect(m_pTwain, SIGNAL(signalImageAcquired(const QImage&)),
            this, SLOT(slotImageAcquiredFromTwain(const QImage&)));

    twIface->selectSource();

    if (!twIface->acquire())
        QMessageBox::critical(this, QString(), i18n("Cannot open scanner device."));

#else /*  WIN32 */

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

#endif /*  WIN32 */
}

void Plugin_AcquireImages::slotImageAcquiredFromTwain(const QImage& img)
{
    img.save("twain.png", "PNG");
}

KIPI::Category Plugin_AcquireImages::category( KAction* action ) const
{
    if ( action == m_action_scanimages )
       return KIPI::ImportPlugin;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification";
    return KIPI::ImportPlugin; // no warning from compiler, please
}
