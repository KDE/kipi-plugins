/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-09-28
 * Description : a tool to export or import image to a KIO accessible
 *               location
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
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

#ifndef PLUGIN_KIOEXPORT_H
#define PLUGIN_KIOEXPORT_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPIKioExportPlugin
{
    class KioExportWindow;
    class KioImportWindow;
}

/**
 * Plugin starter class.
 */
class Plugin_KioExportImport: public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_KioExportImport(QObject *parent, const QVariantList& args);

    KIPI::Category category(KAction* action) const;
    void setup(QWidget* widget);

private Q_SLOTS:

    /**
     * Activates the export tool.
     */
    void slotActivateExport();

    /**
     * Activates the import tool.
     */
    void slotActivateImport();

private:

    KAction                              *m_actionExport;
    KAction                              *m_actionImport;

    KIPIKioExportPlugin::KioExportWindow *m_dlgExport;
    KIPIKioExportPlugin::KioImportWindow *m_dlgImport;
};

#endif //PLUGIN_KIOEXPORT_H
