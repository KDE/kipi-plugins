/* ============================================================
 * File  : plugin_calendar.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
 * Description :
 *
 * Copyright 2003 by Renchi Raju

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

#ifndef PLUGIN_CALENDAR_H
#define PLUGIN_CALENDAR_H

// Lib KIPI includes.

#include <libkipi/plugin.h>

class QProgressDialog;
class QTimer;

class KAction;

class Plugin_Calendar : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_Calendar(QObject *parent,
                    const char* name,
                    const QStringList &args);
    ~Plugin_Calendar();
    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

private:
    
    KAction* m_calendarAction;
    
private slots:

    void slotActivate();
};

#endif /* PLUGIN_CALENDAR_H */
