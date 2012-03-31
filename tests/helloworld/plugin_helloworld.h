/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-16
 * Description : an Hello World plugin.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_HELLOWORLD_H
#define PLUGIN_HELLOWORLD_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

namespace KIPI
{
    class Interface;
}

namespace KIPIHelloWorldPlugin
{

class Plugin_HelloWorld : public KIPI::Plugin
{
    Q_OBJECT

public:

    /** Notice the constructor
        takes two arguments QObject* const parent (the parent of this object),
        and const QStringList& args (the arguments passed).
    */
    Plugin_HelloWorld(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_HelloWorld();

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget*);

private slots:

    /** This is an example slot to which your action is connected.
     */
    void slotActivate();

private:

    /** This is the plugin action that KIPI host application will plug into menu.
     */
    KAction*         m_action;

    /** this is the interface instance to plugin host application. Note that you can get it everywhere in your plugin using
     *  instance of KIPI::PluginLoader singleton which provide a method for that.
     */
    KIPI::Interface* m_iface;
};

}  // namespace KIPIHelloWorldPlugin

#endif // PLUGIN_HELLOWORLD_H
