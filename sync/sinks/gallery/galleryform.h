/* ============================================================
 * File  : galleryform.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-02
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef GALLERYFORM_H
#define GALLERYFORM_H

#include <qcstring.h>
#include <qstring.h>

class KURL;

namespace KIPISyncPlugin
{

typedef enum
{
  Gallery1,
  Gallery2
} GalleryVersion;


class GalleryForm
{
public:

    GalleryForm(GalleryVersion version, QString authToken = "");
    ~GalleryForm();

    void addPair(const QString& name, const QString& value);
    bool addFile(const QString& path, const QString& displayFilename);

    QString    contentType() const;
    QByteArray formData();

private:

    void addPairRaw(const QString& name, const QString& value);

    GalleryVersion mVersion;
    QByteArray m_buffer;
    QCString   mBoundary;
};

}

#endif /* GALLERYMPFORM_H */
