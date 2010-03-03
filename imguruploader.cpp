/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export or import image to imgur.com
 *
 * Copyright (C) 2010 by Marius Orcisk <marius at habarnam dot ro>
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

#include "plugin_imgurexport.h"
#include "imguruploader.h"
// Qt include
#include <QFile>

// KDE include
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/jobuidelegate.h>

using namespace KIPIImgurExportPlugin;

ImgurUploader::ImgurUploader(QObject *parent) :
    QObject(parent)
{
}

bool ImgurUploader::addPhoto (const QString& photoPath)
{
    QFile* photoFile = new QFile(photoPath);
    QByteArray picturedata = photoFile->readAll().toBase64();

    emit signalBusy(true);
    return true;
}
