/* ============================================================
 * File  : gallerympform.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-02
 * Copyright 2004 by Renchi Raju
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

#ifndef GALLERYMPFORM_H
#define GALLERYMPFORM_H

#include <qcstring.h>
#include <qstring.h>

class KURL;

namespace KIPIGalleryExportPlugin
{

class GalleryMPForm
{
public:

    GalleryMPForm();
    ~GalleryMPForm();

    void finish();
    void reset();

    bool addPair(const QString& name, const QString& value);
    bool addFile(const QString& path);
    void setGallery2(bool usegallery2) {m_using_gallery2 = usegallery2;};

    QString    contentType() const;
    QByteArray formData()    const;
    QString    boundary()    const;

private:

    QByteArray m_buffer;
    QCString   m_boundary;
    bool m_using_gallery2;
};

}

#endif /* GALLERYMPFORM_H */
