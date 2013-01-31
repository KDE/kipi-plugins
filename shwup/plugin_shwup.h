/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009      by Timothée Groleau <kde at timotheegroleau dot com>
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

#ifndef PLUGIN_SHWUP_H
#define PLUGIN_SHWUP_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

using namespace KIPI;

namespace KIPIShwupPlugin
{

class SwWindow;

class Plugin_Shwup : public Plugin
{
    Q_OBJECT

public:

    Plugin_Shwup(QObject* const parent, const QVariantList& args);
    ~Plugin_Shwup();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    KAction*  m_actionExport;
    SwWindow* m_dlgExport;
};

} // namespace KIPIShwupPlugin

#endif // PLUGIN_SHWUP_H
