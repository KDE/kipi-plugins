/* ============================================================
 * Authors: Jesper K. Pedersen <blackie@kde.org>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright 2003-2005 by Jesper Pedersen
 * Copyright 2006 by Gilles Caulier
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

#ifndef PLUGIN_TIMEADJUST_H
#define PLUGIN_TIMEADJUST_H

// LibKIPI includes.

#include <libkipi/plugin.h>

class KAction;

class Plugin_TimeAdjust : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_TimeAdjust(QObject *parent, const char* name, const QStringList &args);

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* );

protected slots:

    void slotActivate();

private:

    KAction         *m_actionTimeAjust;

    KIPI::Interface *m_interface;
};

#endif // PLUGIN_TIMEADJUST_H
