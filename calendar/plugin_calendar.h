/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * File  : plugin_calendar.h
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: a plugin to create a monthly calendar using images.
 *
 * Copyright 2003 by Renchi Raju
 * Copyright 2008 by Orgad Shaneh

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

#ifndef __PLUGIN_CALENDAR_H__
#define __PLUGIN_CALENDAR_H__

// LibKIPI includes.

#include <libkipi/plugin.h>

class KAction;

namespace KIPI
{
    class Interface;
}

class Plugin_Calendar : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_Calendar(QObject *parent, const QVariantList &args);
    ~Plugin_Calendar();
    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

protected slots:

    void slotActivate();

private:

    KAction         *m_actionCalendar;

    KIPI::Interface *m_interface;
};

#endif // __PLUGIN_CALENDAR_H__
