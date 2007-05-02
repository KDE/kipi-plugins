/* ============================================================
 * Authors: Jesper K. Pedersen <blackie@kde.org>
 *          Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright 2006 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>

// Local includes.

#include "timeadjustdialog.h"
#include "plugin_timeadjust.h"
#include "plugin_timeadjust.moc"

typedef KGenericFactory<Plugin_TimeAdjust> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_timeadjust,
                            Factory("kipiplugin_timeadjust"))

Plugin_TimeAdjust::Plugin_TimeAdjust(QObject *parent, const char*, const QStringList&)
                 : KIPI::Plugin( Factory::instance(), parent, "TimeAdjust")
{
    kdDebug( 51001 ) << "Plugin_TimeAdjust plugin loaded" << endl;
}

void Plugin_TimeAdjust::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    // this is our action shown in the menubar/toolbar of the mainwindow

    m_actionTimeAjust = new KAction (i18n("Adjust Time && Date..."),
                                     "clock",
                                     0,
                                     this,
                                     SLOT(slotActivate()),
                                     actionCollection(),
                                     "timeadjust");

    addAction(m_actionTimeAjust);

    m_interface = dynamic_cast< KIPI::Interface* >(parent());

    if (!m_interface)
    {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
    }

    KIPI::ImageCollection selection = m_interface->currentSelection();
    m_actionTimeAjust->setEnabled(selection.isValid() &&
                                  !selection.images().isEmpty());

    connect(m_interface, SIGNAL(selectionChanged(bool)),
            m_actionTimeAjust, SLOT(setEnabled(bool)));
}

void Plugin_TimeAdjust::slotActivate()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if (!images.isValid() || images.images().isEmpty())
        return;

    KIPITimeAdjustPlugin::TimeAdjustDialog dlg(m_interface, kapp->activeWindow());
    dlg.setImages(images.images());
    dlg.exec();
}

KIPI::Category Plugin_TimeAdjust::category( KAction* action ) const
{
    if ( action == m_actionTimeAjust )
       return KIPI::IMAGESPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}
