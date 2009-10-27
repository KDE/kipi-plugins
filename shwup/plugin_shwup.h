/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009 by Timothée Groleau <kde at timotheegroleau dot com>
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

namespace KIPIShwupPlugin
{
    class SwWindow;
}

class Plugin_Shwup : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_Shwup(QObject *parent, const QVariantList& args);
    ~Plugin_Shwup();

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget*);

public Q_SLOTS:

    void slotExport();

private:

    KAction                   *m_actionExport;
    KIPIShwupPlugin::SwWindow *m_dlgExport;
};

#endif // PLUGIN_SHWUP_H
