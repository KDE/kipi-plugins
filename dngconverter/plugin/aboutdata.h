/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-11-07
 * Description : DNG converter about data.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ABOUTDATA_H
#define ABOUTDATA_H

#include <QFileInfo>
#include <QString>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>
#include <libkdcraw/version.h>

// Local includes

#include "kpaboutdata.h"

namespace KIPIDNGConverterPlugin
{

class DNGConverterAboutData : public KIPIPlugins::KPAboutData
{

public:

    DNGConverterAboutData()
        : KPAboutData(ki18n("DNG Image Converter"), 0,
                      KAboutData::License_GPL,
                      ki18n("A tool to batch convert RAW camera images to DNG"),
                      ki18n("(c) 2008-2009, Gilles Caulier"))
    {
         addAuthor(ki18n("Gilles Caulier"),
                   ki18n("Author"),
                   "caulier dot gilles at gmail dot com");
    }

    ~DNGConverterAboutData(){}

    static bool isRAWFile(const QString& filePath)
    {

        QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
        QFileInfo fileInfo(filePath);
        if (fileInfo.suffix().toUpper() != QString("DNG"))
        {
            if (rawFilesExt.toUpper().contains( fileInfo.suffix().toUpper() ))
                return true;
        }

        return false;
    }
};

} // namespace KIPIDNGConverterPlugin

#endif /* ABOUTDATA_H */
