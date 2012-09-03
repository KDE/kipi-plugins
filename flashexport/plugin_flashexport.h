/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-20
 * Description : a tool to export images to flash
 *
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#ifndef PLUGIN_FLASHEXPORT_H
#define PLUGIN_FLASHEXPORT_H

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

namespace KIPIFlashExportPlugin
{

class FlashManager;

class Plugin_FlashExport : public Plugin
{
    Q_OBJECT

public:

    Plugin_FlashExport(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_FlashExport();

    void setup(QWidget* const);

private:

    void setupActions();

public Q_SLOTS:

    void slotActivate();

private:

    QWidget*      m_parentWidget;

    KAction*      m_action;

    FlashManager* m_manager;

    Interface*    m_interface;
};

} // namespace KIPIFlashExportPlugin

#endif /* PLUGIN_FLASHEXPORT_H */
