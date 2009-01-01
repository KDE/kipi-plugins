/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to export images to SmugMug web service
 *
 * Copyright (C) 2005-2009 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

#ifndef PLUGIN_SMUGEXPORT_H
#define PLUGIN_SMUGEXPORT_H

// Qt includes.
#include <QVariant>

// LibKIPI includes.
#include <libkipi/plugin.h>

class KAction;

namespace KIPISmugExportPlugin
{
    class SmugWindow;
}

class Plugin_SmugExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_SmugExport(QObject *parent, const QVariantList &args);
    ~Plugin_SmugExport();

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget*);

public slots:

    void slotActivate();

private:

    KAction                                 *m_action;
    KIPISmugExportPlugin::SmugWindow        *m_dlg;
};

#endif // PLUGIN_SMUGEXPORT_H
