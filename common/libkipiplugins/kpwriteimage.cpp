/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-19-09
 * Description : Kipi-Plugins shared library.
 *               Interface to write image data to common picture format.
 * 
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// Qt includes.

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QDataStream>

// KDE includes.

#include <kstandarddirs.h>

// Local includes.
 
#include "kpwriteimage.h"

namespace KIPIPlugins
{

QByteArray KPWriteImage::getICCProfilFromFile(KDcrawIface::RawDecodingSettings::OutputColorSpace colorSpace)
{    
    QString filePath = KStandardDirs::installPath("data") + QString("libkdcraw/profiles/");

    switch(colorSpace)
    {
        case KDcrawIface::RawDecodingSettings::SRGB:
        {
            filePath.append("srgb.icm");
            break;
        }
        case KDcrawIface::RawDecodingSettings::ADOBERGB:
        {
            filePath.append("adobergb.icm");
            break;
        }
        case KDcrawIface::RawDecodingSettings::WIDEGAMMUT:
        {
            filePath.append("widegamut.icm");
            break;
        }
        case KDcrawIface::RawDecodingSettings::PROPHOTO:
        {
            filePath.append("prophoto.icm");
            break;
        }
        default:
            break;
    }

    if ( filePath.isEmpty() ) 
        return QByteArray();

    QFile file(filePath);
    if ( !file.open(QIODevice::ReadOnly) ) 
        return false;
    
    QByteArray data;
    data.resize(file.size());
    QDataStream stream( &file );
    stream.readRawData(data.data(), data.size());
    file.close();

    return data;
}

}  // NameSpace KIPIPlugins
