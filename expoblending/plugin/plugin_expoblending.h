/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_EXPOBLENDING_H
#define PLUGIN_EXPOBLENDING_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class QWidget;

class KAction;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIExpoBlendingPlugin
{

class Manager;

class Plugin_ExpoBlending : public Plugin
{
    Q_OBJECT

public:

    Plugin_ExpoBlending(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_ExpoBlending();

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

} // namespace KIPIExpoBlendingPlugin

#endif /* PLUGIN_EXPOBLENDING_H */
