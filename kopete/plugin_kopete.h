/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-12-04
 * Description : a kipi plugin to export images to a Kopete contact
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

#ifndef PLUGIN_KOPETE_H
#define PLUGIN_KOPETE_H

// Qt includes

#include <QVariant>
#include <QDBusInterface>

// LibKIPI includes

#include <libkipi/plugin.h>

class QSignalMapper;
class KAction;
class KMenu;

class KJob;

using namespace KIPI;

namespace KIPIKopetePlugin
{

class Plugin_Kopete : public Plugin
{
    Q_OBJECT

public:

    Plugin_Kopete(QObject* const parent, const QVariantList& args);
    ~Plugin_Kopete();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotAboutToShowMenu();
    void slotTransferFiles(const QString& contactId);

private:

    bool kopeteRunning();

private:

    QDBusInterface m_kopeteDBus;
    QDBusInterface m_kopeteDBusTest;
    KAction*       m_actionExport;
    QSignalMapper* m_signalMapper;
};

} // namespace KIPIKopetePlugin

#endif // PLUGIN_KOPETE_H
