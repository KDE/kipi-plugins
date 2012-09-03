/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to import/export images to/from 
 *               SmugMug web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

using namespace KIPI;

namespace KIPISmugPlugin
{

class SmugWindow;

class Plugin_Smug : public Plugin
{
    Q_OBJECT

public:

    Plugin_Smug(QObject* const parent, const QVariantList& args);
    ~Plugin_Smug();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotExport();
    void slotImport();

private:

    void setupActions();

private:

    KAction*    m_actionExport;
    KAction*    m_actionImport;

    SmugWindow* m_dlgExport;
    SmugWindow* m_dlgImport;
};

} // namespace KIPISmugPlugin

#endif // PLUGIN_SMUG_H
