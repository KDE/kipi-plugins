/* ============================================================
 * File  : plugin_printwizard.h
 * Author: Todd Shoemaker <todd@theshoemakers.net>
 * Date  : 2003-09-30
 * Description :
 *
 * Copyright 2003 by Todd Shoemaker

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_PRINTWIZARD_H
#define PLUGIN_PRINTWIZARD_H

#include <libkipi/plugin.h>

class KAction;

class Plugin_PrintWizard : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_PrintWizard(QObject *parent,
                     const char* name,
                     const QStringList &args);
    ~Plugin_PrintWizard();

public slots:

    void slotActivate();
    void slotItemsSelected(bool selected);
    virtual KIPI::Category category() const;

private:
    KAction* printAction;
    KIPI::Interface* m_interface;
};

#endif
