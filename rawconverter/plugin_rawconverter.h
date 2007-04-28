/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2003-01-31
 * Description : a kipi plugin to convert Raw file in single 
 *               or batch mode.
 *
 * Copyright 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_RAWCONVERTER_H
#define PLUGIN_RAWCONVERTER_H

// LibKIPi includes.

#include <libkipi/plugin.h>

class KAction;

class Plugin_RawConverter : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_RawConverter(QObject *parent,
                        const char* name,
                        const QStringList &args);
    ~Plugin_RawConverter();
    
    KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );
    
private:

    bool checkBinaries();
    bool isRAWFile(const QString& filePath);

private slots:

    void slotActivateSingle();
    void slotActivateBatch();

private:

    KAction *singleAction_;
    KAction *batchAction_;
};

#endif /* PLUGIN_RAWCONVERTER_H */
