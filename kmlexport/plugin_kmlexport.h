/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-05-16
 * Description : A plugin to create KML files to present images with coordinates.
 *
 * Copyright 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_KMLEXPORT_H
#define PLUGIN_KMLEXPORT_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KActionMenu;
class KAction;

namespace KIPI
{
class Interface;
} // namespace KIPI

class Plugin_KMLExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_KMLExport(QObject* parent, const QVariantList& args);

    KIPI::Category category( KAction* action ) const;
    void setup( QWidget* );

protected Q_SLOTS:

    void slotKMLGenerate();
    void slotKMLExport();

private:

    KAction*         m_actionKMLExport;

    KIPI::Interface* m_interface;
};

#endif // PLUGIN_KMLEXPORT_H
