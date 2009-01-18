/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : Jan 18, 2009
 * Description : XXXXXXXXXXXXXXXXXXXXXXXx
 *
 * Copyright (C) 2009 by andi <xxxxxxxxxxxxx>
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
 *
 * ============================================================ */

#include "SaveMethods.h"

// Qt includes.

#include <QDir>
#include <QFileInfo>

// KDE includes.

#include <kurl.h>

namespace KIPIRemoveRedEyesPlugin
{

QString SaveSubfolder::savePath(const QString& path)
{
    QFileInfo info(path);
    KUrl savePath(info.path());

    QString subfolder("corrected");
    savePath.addPath(subfolder);

    // check if subfolder exists
    if (!QDir(savePath.path()).exists())
        QDir(info.path()).mkdir(subfolder);

    savePath.addPath(info.fileName());
    return savePath.path();
}

QString SaveSuffix::savePath(const QString& path)
{
    QFileInfo info(path);
    KUrl savePath(info.path());

    QString suffix("_rre");

    QString file = info.baseName();
    file.append(suffix);
    file.append(".");
    file.append(info.suffix());

    savePath.addPath(file);
    return savePath.path();
}

QString SaveOverwrite::savePath(const QString& path)
{
    return path;
}

}
