/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_FLASHEXPORT_H
#define PLUGIN_FLASHEXPORT_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

namespace KIPI
{
class Interface;
} // namespace KIPI

class KAction;

class Plugin_FlashExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_FlashExport(QObject *parent, const QVariantList &args);

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

private Q_SLOTS:

    void slotActivate();

private:

    KAction         *m_actionFlashExport;
    KIPI::Interface *m_interface;
};

#endif // PLUGIN_FLASHEXPORT_H
