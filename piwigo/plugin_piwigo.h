/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2014-09-30
 * Description : a plugin to export to a remote Piwigo server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010-2014 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#ifndef PLUGIN_PIWIGO_H
#define PLUGIN_PIWIGO_H

// Qt includes

#include <QVariantList>

// Libkipi includes

#include <KIPI/Plugin>

class QAction;

using namespace KIPI;

namespace KIPIPiwigoExportPlugin
{

class Piwigo;

class Plugin_PiwigoExport : public Plugin
{
    Q_OBJECT

public:

    Plugin_PiwigoExport(QObject* const parent, const QVariantList& args);
    ~Plugin_PiwigoExport();

    void setup(QWidget* const) override;

public Q_SLOTS:

    void slotSync();

private:

    void setupActions();

private:

    QAction * m_action;
    Piwigo*  m_pPiwigo;
};

} // namespace KIPIPiwigoExportPlugin

#endif /* PLUGIN_PIWIGO_H */
