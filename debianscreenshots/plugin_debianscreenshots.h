/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#ifndef PLUGIN_DEBIANSCREENSHOTS_H
#define PLUGIN_DEBIANSCREENSHOTS_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

//#if KDE_IS_VERSION(4,3,60)
//#include <libkipi/exportinterface.h>
//#endif

class KAction;

namespace KIPIDebianScreenshotsPlugin
{
    class DsWindow;
}

class KJob;

class Plugin_DebianScreenshots
    : public KIPI::Plugin
//#if KDE_IS_VERSION(4,3,60)
//    , public KIPI::ExportInterface
//#endif
{
Q_OBJECT
//#if KDE_IS_VERSION(4,3,60)
//Q_INTERFACES( KIPI::ExportInterface )
//#endif

public:

    Plugin_DebianScreenshots(QObject* parent, const QVariantList& args);
    ~Plugin_DebianScreenshots();

    KIPI::Category category(KAction* action) const;
    void setup(QWidget*);
//    virtual KJob* exportFiles(const QString& album);

public Q_SLOTS:

    void slotExport();

private:

    KAction*                               m_actionExport;
    KIPIDebianScreenshotsPlugin::DsWindow* m_dlgExport;
};

#endif // PLUGIN_DEBIANSCREENSHOTS_H
