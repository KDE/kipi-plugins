/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

// Libkipi includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPIGoogleServicesPlugin
{
    class GSWindow;
}

class KJob;

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
    void slotPicasaExport();
    void slotPicasaImport();

private:

    void setupActions();

private:

    KAction*  m_actionGDriveExport;
    KAction*  m_actionPicasaExport;
    KAction*  m_actionPicasaImport;

    GSWindow* m_dlgGDriveExport;
    GSWindow* m_dlgPicasaExport;
    GSWindow* m_dlgPicasaImport;
};

} // namespace KIPIGoogleServicesPlugin

#endif /* PLUGIN_GOOGLESERVICES_H */
