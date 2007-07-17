/* ============================================================
 * File  : plugin_picasawebexport.h
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2007-16-07
 * Description :
 *
 * Copyright 2007 by Vardhman Jain <vardhman @ gmail.com>

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

#ifndef PLUGIN_PICASAWEBEXPORT_H
#define PLUGIN_PICASAWEBEXPORT_H

#include "picasawebwindow.h"

// libKIPI includes.
#include <libkipi/plugin.h>
class KAction;

using namespace KIPIPicasawebExportPlugin;
class Plugin_PicasawebExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_PicasawebExport(QObject *parent,
                         const char* name,
                         const QStringList &args);
    ~Plugin_PicasawebExport();
    
    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* );

public slots:

    void slotActivate();

private:
    PicasawebWindow *dlg;
    KAction *m_action;
};

#endif
