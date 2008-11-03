/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-30-09
 * Description : a kipi plugin to print images
 *
 * Copyright 2003 by Todd Shoemaker <todd@theshoemakers.net>
 * Copyright 2007-2008 by Angelo Naselli <anaselli at linux dot it>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_PRINTWIZARD_H
#define PLUGIN_PRINTWIZARD_H

// Qt includes.

#include <QVariant>

// LibKIPI includes.

#include <libkipi/plugin.h>

class KAction;

namespace KIPI
{
class Interface;
} // namespace KIPI

class Plugin_PrintWizard : public KIPI::Plugin
{
    Q_OBJECT

public:
    Plugin_PrintWizard(QObject *parent, const QVariantList &args);

    ~Plugin_PrintWizard();
    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

public slots:

    void slotActivate();

private:
    KAction         *m_printAction;
    KIPI::Interface *m_interface;
};

#endif // PLUGIN_PRINTWIZARD_H
