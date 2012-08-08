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

class KAction;

using namespace KIPI;

namespace KIPIDebianScreenshotsPlugin
{

class DsWindow;

class Plugin_DebianScreenshots : public Plugin
{
    Q_OBJECT

public:

    Plugin_DebianScreenshots(QObject* const parent, const QVariantList& args);
    ~Plugin_DebianScreenshots();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    KAction*   m_actionExport;
    DsWindow*  m_dlgExport;
};

} // namespace KIPIDebianScreenshotsPlugin

#endif // PLUGIN_DEBIANSCREENSHOTS_H
