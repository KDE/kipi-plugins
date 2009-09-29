/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-09-28
 * Description : a tool to export image to a KIO accessible
 *               location
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
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

#ifndef PLUGIN_KIOEXPORT_H
#define PLUGIN_KIOEXPORT_H

#include <QVariant>

#include <libkipi/plugin.h>

class KAction;

namespace KIPI
{
class Interface;
}

class Plugin_KioExport: public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_KioExport(QObject *parent, const QVariantList&args);

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget* widget);

private Q_SLOTS:

void slotActivate();

private:

    KAction *m_action;
};

#endif //PLUGIN_KIOEXPORT_H
