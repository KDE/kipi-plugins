/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-05-16
 * Description : A plugin to create KML files to present images with coordinates.
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

class KAction;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIKMLExportPlugin
{

class Plugin_KMLExport : public Plugin
{
    Q_OBJECT

public:

    Plugin_KMLExport(QObject* const parent, const QVariantList& args);
    ~Plugin_KMLExport();

    void setup(QWidget* const);

protected Q_SLOTS:

    void slotKMLGenerate();
    void slotKMLExport();

private:

    void setupActions();

private:

    KAction*   m_actionKMLExport;

    Interface* m_interface;
};

} // namespace KIPIKMLExportPlugin

#endif // PLUGIN_KMLEXPORT_H
