/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to import/export images to/from 
 *               SmugMug web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_SMUG_H
#define PLUGIN_SMUG_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPISmugPlugin
{
    class SmugWindow;
}

class Plugin_Smug : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_Smug(QObject *parent, const QVariantList &args);
    ~Plugin_Smug();

    KIPI::Category category(KAction* action) const;
    void setup(QWidget*);

public Q_SLOTS:

    void slotExport();
    void slotImport();

private:

    KAction                     *m_actionExport;
    KAction                     *m_actionImport;

    KIPISmugPlugin::SmugWindow  *m_dlgExport;
    KIPISmugPlugin::SmugWindow  *m_dlgImport;
};

#endif // PLUGIN_SMUG_H
