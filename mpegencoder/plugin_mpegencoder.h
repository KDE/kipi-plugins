/* ============================================================
 * File  : plugin_mpegencoder.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-02
 * Description :
 *
 * Copyright 2003 by Gilles Caulier

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

#ifndef PLUGIN_MPEGENCODER_H
#define PLUGIN_MPEGENCODER_H

// KIPI includes

#include <libkipi/plugin.h>

class KAction;

class Plugin_Mpegencoder : public KIPI::Plugin
{
Q_OBJECT

public:

    Plugin_Mpegencoder(QObject *parent, const char* name, const QStringList &args);
    virtual ~Plugin_Mpegencoder();
    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );
    
public slots:

    void slotActivate();

private:
    KAction* m_actionMPEGEncoder;
};


#endif /* PLUGIN_MPEGENCODER_H */
