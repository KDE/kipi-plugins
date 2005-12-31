/* ============================================================
 * File  : plugin_simpleviewer.h
 * Author: Joern Ahrens <joern.ahrens@kdemail.net>
 * Date  : 2005-12-19
 * Description :
 *
 * Copyright 2005 by Joern Ahrens
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

#ifndef PLUGIN_SIMPLEVIEWER_H
#define PLUGIN_SIMPLEVIEWER_H

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

class Plugin_SimpleViewer : public KIPI::Plugin
{
    Q_OBJECT

public:

    // Notice the constructor
    // takes three arguments QObject *parent (the parent of this object),
    // const char* name (the name of this object) and
    // const QStringList &args (the arguments passed).
    Plugin_SimpleViewer(QObject *parent,
                      const char* name,
                      const QStringList &args);

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

private slots:

    //This is an example slot to which your action is connected.
    void slotActivate();

private:
    KAction         *m_actionSimpleViewer;
    KIPI::Interface *m_interface;
};

#endif

