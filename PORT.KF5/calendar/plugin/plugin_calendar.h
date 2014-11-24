/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : plugin entry point.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
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

#ifndef PLUGIN_CALENDAR_H
#define PLUGIN_CALENDAR_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPICalendarPlugin
{

class Plugin_Calendar : public Plugin
{
    Q_OBJECT

public:

    Plugin_Calendar(QObject* const parent, const QVariantList& args);
    ~Plugin_Calendar();

    virtual void setup(QWidget* const widget);

protected Q_SLOTS:

    void slotActivate();

private:

    void setupActions();

private:

    KAction* m_actionCalendar;
};

}  // NameSpace KIPICalendarPlugin

#endif // PLUGIN_CALENDAR_H
