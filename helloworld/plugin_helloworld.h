/* ============================================================
 * File  : plugin_helloworld.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-03-10
 * Description :
 *
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef PLUGIN_HELLOWORLD_H
#define PLUGIN_HELLOWORLD_H

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

class Plugin_HelloWorld : public KIPI::Plugin
{
    Q_OBJECT

public:

    // Notice the constructor
    // takes three arguments QObject *parent (the parent of this object),
    // const char* name (the name of this object) and
    // const QStringList &args (the arguments passed).
    Plugin_HelloWorld(QObject *parent,
                      const char* name,
                      const QStringList &args);

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

private slots:

    //This is an example slot to which your action is connected.
    void slotActivate();

private:
    KAction         *m_actionHelloWorld;
    KIPI::Interface *m_interface;
};

#endif
