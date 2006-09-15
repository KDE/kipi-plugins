/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2006-09-15
 * Description : Exiv2 library interface
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

#ifndef EXIV2IFACE_H
#define EXIV2IFACE_H

// QT includes.

#include <qcstring.h>
#include <qstring.h>

namespace KIPIRawConverterPlugin
{

class Exiv2IfacePriv;

class Exiv2Iface
{

public:

    Exiv2Iface();
    ~Exiv2Iface();
    
    bool load(const QString& filePath);
    bool save(const QString& filePath);

    QByteArray getComments() const;
    QByteArray getExif() const;
    QByteArray getIptc(bool addIrbHeader=false) const;

    void setComments(const QByteArray& data);
    void setExif(const QByteArray& data);
    void setIptc(const QByteArray& data);

    bool setImageProgramId(const QString& program, const QString& version);

private:

    Exiv2IfacePriv *d;
};

}  // NameSpace KIPIRawConverterPlugin

#endif /* EXIV2IFACE_H */
