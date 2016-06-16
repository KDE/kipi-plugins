/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013      by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2008-2016 by Caulier Gilles <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_GOOGLESERVICES_H
#define PLUGIN_GOOGLESERVICES_H

// Qt includes

#include <QVariant>
#include <QTemporaryDir>

// Libkipi includes

#include <KIPI/Plugin>

class QAction;

namespace KIPIGoogleServicesPlugin
{
    class GSWindow;
}

using namespace KIPI;

namespace KIPIGoogleServicesPlugin
{

class Plugin_GoogleServices:public Plugin
{
    Q_OBJECT

public:

    Plugin_GoogleServices(QObject* const parent, const QVariantList& args);
    ~Plugin_GoogleServices();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotGDriveExport();
    void slotGPhotoExport();
    void slotGPhotoImport();

private:

    void setupActions();
    QString getTempDirPath();

private:

    QAction*       m_actionGDriveExport;
    QAction*       m_actionGPhotoExport;
    QAction*       m_actionGPhotoImport;

    GSWindow*      m_dlgGDriveExport;
    GSWindow*      m_dlgGPhotoExport;
    GSWindow*      m_dlgGPhotoImport;

    QTemporaryDir* m_temporaryDir;
};

} // namespace KIPIGoogleServicesPlugin

#endif /* PLUGIN_GOOGLESERVICES_H */
