/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#ifndef PLUGIN_FBEXPORT_H
#define PLUGIN_FBEXPORT_H

// Qt includes.
#include <QVariant>

// LibKIPI includes.
#include <libkipi/plugin.h>

class KAction;

namespace KIPIFbExportPlugin
{
    class FbWindow;
}

class Plugin_FbExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_FbExport(QObject *parent, const QVariantList &args);
    ~Plugin_FbExport();

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget*);

public slots:

    void slotActivate();

private:

    KAction                         *m_action;
    KIPIFbExportPlugin::FbWindow    *m_dlg;
};

#endif // PLUGIN_FBEXPORT_H
