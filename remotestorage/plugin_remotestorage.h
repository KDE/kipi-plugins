/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef PLUGIN_REMOTESTORAGE_H
#define PLUGIN_REMOTESTORAGE_H

// Qt includes

#include <QVariant>

// Libkipi includes

#include <KIPI/Plugin>

class QAction;

using namespace KIPI;

namespace KIPIRemoteStoragePlugin
{

class KioExportWindow;
class KioImportWindow;

class Plugin_RemoteStorage: public Plugin
{
    Q_OBJECT

public:

    Plugin_RemoteStorage(QObject* const parent, const QVariantList& args);
    ~Plugin_RemoteStorage();

    void setup(QWidget* const widget);

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

    void setupActions();

private:

    QAction *        m_actionExport;
    QAction *        m_actionImport;

    KioExportWindow* m_dlgExport;
    KioImportWindow* m_dlgImport;
};

} // namespace KIPIRemoteStoragePlugin

#endif // PLUGIN_REMOTESTORAGE_H
