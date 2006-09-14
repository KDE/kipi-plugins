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

#ifndef DCRAWBINARY_H
#define DCRAWBINARY_H

// Qt includes.

#include <qstring.h>
#include <qobject.h>

class KProcess;

namespace KIPIRawConverterPlugin
{

class DcrawBinaryPriv;

class DcrawBinary : public QObject
{
    Q_OBJECT

public:

    DcrawBinary();
    ~DcrawBinary();

    const char *path();
    bool isAvailable() const;
    QString version() const;
    bool versionIsRight() const;
    QString minimalVersion() const;

    void checkSystem();

private slots:

    void slotReadStderrFromDcraw(KProcess*, char*, int);

private:

    DcrawBinaryPriv    *d;
};

} // namespace KIPIRawConverterPlugin

#endif  // DCRAWBINARY_H
