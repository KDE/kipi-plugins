/* ============================================================
 * Author: Marcel Wiesweg <marcel.wiesweg@gmx.de>
 * Date  : 2006-04-13
 * Description : Autodetect dcraw binary program and version
 *
 * Copyright 2006 by Marcel Wiesweg
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

#include "dcrawbinary.h"
#include "dcrawbinary.moc"

namespace KIPIRawConverterPlugin
{

class DcrawBinaryPriv
{
public:

    DcrawBinaryPriv()
    {
        available = false;
        version   = QString::null;
    }

    bool    available;

    QString version;
};

DcrawBinary::DcrawBinary()
           : QObject()
{
    d = new DcrawBinaryPriv;
    checkSystem();
}

DcrawBinary::~DcrawBinary()
{
    delete d;
}

void DcrawBinary::checkSystem()
{
    KProcess process;
    process.clearArguments();
    process << path();    

    connect(&process, SIGNAL(receivedStderr(KProcess *, char*, int)),
            this, SLOT(slotReadStderrFromDcraw(KProcess*, char*, int)));

    d->available = process.start(KProcess::Block, KProcess::Stderr);
}

void DcrawBinary::slotReadStderrFromDcraw(KProcess*, char* buffer, int buflen)
{
    // The dcraw ouput look like this : Raw Photo Decoder "dcraw" v8.30...
    QString dcrawHeader("Raw Photo Decoder \"dcraw\" v");

    QString stdErr    = QString::fromLocal8Bit(buffer, buflen);
    QString firstLine = stdErr.section('\n', 1, 1);    

    if (firstLine.startsWith(dcrawHeader))
    {
        d->version = firstLine.remove(0, dcrawHeader.length());    
        kdDebug() << "Found dcraw version: " << version() << endl;    
    }
}

const char *DcrawBinary::path()
{
    return "kipidcraw";
}

bool DcrawBinary::isAvailable() const
{
    return d->available;
}

QString DcrawBinary::version() const
{
    return d->version;
}

bool DcrawBinary::versionIsRight() const
{
    if (d->version.isNull() || !isAvailable())
        return false;

    if (d->version.toFloat() >= minimalVersion().toFloat())
        return true;

    return false;
}

QString DcrawBinary::minimalVersion() const
{
    return QString("8.16");
}

}  // namespace KIPIRawConverterPlugin

