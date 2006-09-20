/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-19
 * Description : Autodetect gpsbabel binary program and version
 *
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

#ifndef GPSBABELBINARY_H
#define GPSBABELBINARY_H

// Qt includes.

#include <qstring.h>
#include <qobject.h>

class KProcess;

namespace KIPIGPSSyncPlugin
{

class GPSBabelBinaryPriv;

class GPSBabelBinary : public QObject
{
    Q_OBJECT

public:

    GPSBabelBinary();
    ~GPSBabelBinary();

    static const char *path();
    bool isAvailable() const;
    QString version() const;
    bool versionIsRight() const;
    QString minimalVersion() const;

    void checkSystem();

private slots:

    void slotReadStdoutFromGPSBabel(KProcess*, char*, int);

private:

    GPSBabelBinaryPriv    *d;
};

} // namespace KIPIGPSSyncPlugin

#endif  // GPSBABELBINARY_H
