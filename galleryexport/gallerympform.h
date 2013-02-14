/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GALLERYMPFORM_H
#define GALLERYMPFORM_H

// Qt includes

#include <QByteArray>
#include <QString>

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
    bool addFile(const QString& path, const QString& displayFilename);

    QString    contentType() const;
    QByteArray formData()    const;
    QString    boundary()    const;

private:

    bool addPairRaw(const QString& name, const QString& value);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIGalleryExportPlugin

#endif /* GALLERYMPFORM_H */
