/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Dropbox web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

// local includes

#include "mpform.h"

// Qt includes

#include <QFile>

// KDE includes

#include <kdebug.h>

namespace KIPIDropboxPlugin
{

MPForm::MPForm()
{
}

MPForm::~MPForm()
{
}

bool MPForm::addFile(const QString &imgPath)
{
    QFile file(imgPath);

    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    m_buffer = file.readAll();

    return true;
}

QByteArray MPForm::formData()
{
    return m_buffer;
}

} // namespace KIPIDropboxPlugin
