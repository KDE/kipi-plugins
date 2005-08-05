/* ============================================================
 * File  : mpform.h
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2005-07-07
 * Copyright 2005 by Vardhman Jain
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
 * ============================================================ */

#ifndef MPFORM_H
#define MPFORM_H

#include <qcstring.h>
#include <qstring.h>

class KURL;

namespace KIPIFlickrExportPlugin
{

class MPForm
{
public:

    MPForm();
    ~MPForm();

    void finish();
    void reset();

    bool addPair(const QString& name, const QString& value);
    bool addFile(const QString& name, const QString& path);

    QString    contentType() const;
    QByteArray formData()    const;
    QString    boundary()    const;
    
private:

    QByteArray m_buffer;
    QCString   m_boundary;
};

}

#endif /* MPFORM_H */
