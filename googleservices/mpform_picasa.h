/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#ifndef MPFORM_PICASA_H
#define MPFORM_PICASA_H

// Qt includes

#include <QByteArray>
#include <QString>

namespace KIPIGoogleServicesPlugin
{

class MPForm_Picasa
{

public:

    MPForm_Picasa();
    ~MPForm_Picasa();

    void finish();
    void reset();

    bool addPair(const QString& name, const QString& value, const QString& contentType=QString());
    bool addFile(const QString& name, const QString& path);

    QString    contentType() const;
    QByteArray formData()    const;
    QString    boundary()    const;

private:

    QByteArray  m_buffer;
    QByteArray  m_boundary;
};

} // namespace KIPIGoogleServicesPlugin

#endif /* MPFORM_PICASA_H */
