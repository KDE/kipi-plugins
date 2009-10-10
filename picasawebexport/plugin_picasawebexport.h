/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-17-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_PICASAWEBEXPORT_H
#define PLUGIN_PICASAWEBEXPORT_H

// LibKIPI includes

#include <libkipi/plugin.h>

// Local includes

#include "picasawebwindow.h"

class KAction;

using namespace KIPIPicasawebExportPlugin;

class Plugin_PicasawebExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_PicasawebExport(QObject *parent, const QVariantList& args);
    ~Plugin_PicasawebExport();

    KIPI::Category category(KAction* action) const;
    void setup(QWidget*);

public Q_SLOTS:

    void slotActivate();

private:

    KAction         *m_actionExport;

    PicasawebWindow *m_dlgExport;
};

#endif // PLUGIN_PICASAWEBEXPORT_H
