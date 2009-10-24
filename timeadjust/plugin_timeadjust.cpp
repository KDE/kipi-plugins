/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_timeadjust.h"
#include "plugin_timeadjust.moc"

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

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes

#include "timeadjustdialog.h"

K_PLUGIN_FACTORY( TimeAdjustFactory, registerPlugin<Plugin_TimeAdjust>(); )
K_EXPORT_PLUGIN ( TimeAdjustFactory("kipiplugin_timeadjust") )

Plugin_TimeAdjust::Plugin_TimeAdjust(QObject *parent, const QVariantList&)
                 : KIPI::Plugin( TimeAdjustFactory::componentData(), parent, "TimeAdjust")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_TimeAdjust plugin loaded";
}

Plugin_TimeAdjust::~Plugin_TimeAdjust()
{
}

void Plugin_TimeAdjust::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_actionTimeAjust = actionCollection()->addAction("timeadjust");
    m_actionTimeAjust->setText(i18n("Adjust Time && Date..."));
    m_actionTimeAjust->setIcon(KIcon("timeadjust"));

    connect(m_actionTimeAjust, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_actionTimeAjust);

    m_interface = dynamic_cast< KIPI::Interface* >(parent());
    if (!m_interface)
    {
       kError() << "Kipi interface is null!";
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
       return KIPI::ImagesPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ImagesPlugin; // no warning from compiler, please
}
