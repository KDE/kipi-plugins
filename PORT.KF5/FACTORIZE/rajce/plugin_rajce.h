/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_RAJCE_H
#define PLUGIN_RAJCE_H

// Qt includes

#include <QVariant>

// Libkipi includes

#include <KIPI/Plugin>

// Local includes

#include "rajcewindow.h"

using namespace KIPI;

class QAction;

namespace KIPIRajcePlugin
{

class Plugin_Rajce : public Plugin
{
    Q_OBJECT

public:

    Plugin_Rajce(QObject* const parent, const QVariantList& args);
    ~Plugin_Rajce();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    QAction *    m_actionExport;
    RajceWindow* m_dlgExport;
};

} // namespace KIPIRajcePlugin

#endif // PLUGIN_RAJCE_H
