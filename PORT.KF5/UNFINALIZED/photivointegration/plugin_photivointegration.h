/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-06-14
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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

#ifndef PLUGIN_PHOTIVOINTEGRATION_H
#define PLUGIN_PHOTIVOINTEGRATION_H

// Qt includes

#include <QVariant>

// Libkipi includes

#include <KIPI/Plugin>

// local includes

#include "piwindow.h"

// forward declaration:
namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIPhotivoIntegrationPlugin
{

// forward declaration:
class PIWindow;

// ----------------------------------------------------------------------------

class Plugin_PhotivoIntegration : public Plugin
{
    Q_OBJECT

public:

    /** Notice the constructor
        takes two arguments QObject* const parent (the parent of this object),
        and const QStringList& args (the arguments passed).
    */
    Plugin_PhotivoIntegration(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_PhotivoIntegration();

    void setup(QWidget* const);

private slots:

    /** This is an example slot to which your action is connected.
     */
    void slotActivate();

private:

    void setupActions();

private:

    /** This is the plugin action that KIPI host application will plug into menu.
     */
    QAction *   m_action;

    /** this is the interface instance to plugin host application. Note that you can get it everywhere in your plugin using
     *  instance of KIPI::PluginLoader singleton which provide a method for that.
     */
    Interface* m_iface;

    /** PIWindow instance */
    PIWindow*  piWin;
};

// ----------------------------------------------------------------------------

}  // namespace KIPIPhotivoIntegrationPlugin

#endif // PLUGIN_PHOTIVOINTEGRATION_H
