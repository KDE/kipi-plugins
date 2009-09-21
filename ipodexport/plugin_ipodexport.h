/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz@kde.org>
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

#ifndef PLUGIN_IPODEXPORT_H
#define PLUGIN_IPODEXPORT_H

#include <QVariant>

#include <libkipi/plugin.h>

class KAction;

namespace KIPI
{
    class Interface;
}

class Plugin_iPodExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_iPodExport( QObject *parent, const QVariantList&args);
    ~Plugin_iPodExport() {};

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget* widget);

private Q_SLOTS:
    void slotImageUpload();

private:
    KAction         *m_actionImageUpload;
    KIPI::Interface *m_interface;
};

#endif //PLUGIN_IPODEXPORT_H
