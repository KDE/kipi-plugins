/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef MPFORM_H
#define MPFORM_H

// Qt includes

#include <QByteArray>
#include <QString>

namespace KIPIFlickrExportPlugin
{

class MPForm
{

public:

    MPForm();
    ~MPForm();

    void finish();
    void reset();

    bool addPair(const QString& name, const QString& value, const QString& type);
    bool addFile(const QString& name, const QString& path);

    QString    contentType() const;
    QByteArray formData()    const;
    QString    boundary()    const;

private:

    QByteArray m_buffer;
    QByteArray m_boundary;
};

} // namespace KIPIFlickrExportPlugin

#endif /* MPFORM_H */
