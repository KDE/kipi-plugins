/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-19
 * Description : Autodetect gpsbabel binary program and version
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kapplication.h>
#include <kprocess.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kaboutdata.h>

// Local includes

#include "gpsbabelbinary.h"
#include "gpsbabelbinary.moc"

namespace KIPIGPSSyncPlugin
{

class GPSBabelBinaryPriv
{
public:

    GPSBabelBinaryPriv()
    {
        available = false;
        version   = QString::null;
    }

    bool    available;

    QString version;
};

GPSBabelBinary::GPSBabelBinary()
           : QObject()
{
    d = new GPSBabelBinaryPriv;
    checkSystem();
}

GPSBabelBinary::~GPSBabelBinary()
{
    delete d;
}

void GPSBabelBinary::checkSystem()
{
    KProcess process;
    process.clearArguments();
    process << path() << "-V";    

    connect(&process, SIGNAL(receivedStdout(KProcess *, char*, int)),
            this, SLOT(slotReadStdoutFromGPSBabel(KProcess*, char*, int)));

    d->available = process.start(KProcess::Block, KProcess::Stdout);
}

void GPSBabelBinary::slotReadStdoutFromGPSBabel(KProcess*, char* buffer, int buflen)
{
    // The dcraw output look like this : GPSBabel Version 1.2.5
    QString headerStarts("GPSBabel Version ");

    QString stdOut    = QString::fromLocal8Bit(buffer, buflen);
    QString firstLine = stdOut.section('\n', 1, 1);    

    if (firstLine.startsWith(headerStarts))
    {
        d->version = firstLine.remove(0, headerStarts.length());   
        kdDebug( 51001 ) << "Found gpsbabel version: " << version() << endl;    
    }
}

const char *GPSBabelBinary::path()
{
    return "gpsbabel";
}

bool GPSBabelBinary::isAvailable() const
{
    return d->available;
}

QString GPSBabelBinary::version() const
{
    return d->version;
}

bool GPSBabelBinary::versionIsRight() const
{
    if (d->version.isNull() || !isAvailable())
        return false;

    if (d->version.toFloat() >= minimalVersion().toFloat())
        return true;

    return false;
}

QString GPSBabelBinary::minimalVersion() const
{
    return QString("1.2.5");
}

}  // namespace KIPIGPSSyncPlugin
