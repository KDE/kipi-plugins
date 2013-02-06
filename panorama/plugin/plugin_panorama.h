/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2013 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_PANORAMA_H
#define PLUGIN_PANORAMA_H

// Qt includes

#include <QVariant>
#include <QPointer>

// LibKIPI includes

#include <libkipi/plugin.h>

class QWidget;

class KAction;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIPanoramaPlugin
{

class Manager;

class Plugin_Panorama : public Plugin
{
    Q_OBJECT

public:

    Plugin_Panorama(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_Panorama();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotActivate();

private:

    void setupActions();

private:

    QWidget*   m_parentWidget;
    KAction*   m_action;
    Manager*   m_manager;
    Interface* m_interface;
};

} // namespace KIPIPanoramaPlugin

#endif /* PLUGIN_PANORAMA_H */
