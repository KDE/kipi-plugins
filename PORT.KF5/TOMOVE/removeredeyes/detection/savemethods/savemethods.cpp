/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-01-18
 * Description : different save methods for red eye correction
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "savemethods.h"

// Qt includes

#include <QDir>
#include <QFileInfo>

// KDE includes

#include <QUrl>

namespace KIPIRemoveRedEyesPlugin
{

QString SaveSubfolder::savePath(const QString& path, const QString& extra) const
{
    QFileInfo info(path);
    QUrl savePath = QUrl::fromLocalFile(info.path());
    savePath.setPath(savePath.path() + '/' + extra);

    // check if subfolder exists
    if (!QDir(savePath.path()).exists())
    {
        QDir(info.path()).mkdir(extra);
    }

    savePath.setPath(savePath.path() + '/' + info.fileName());
    return savePath.path();
}

// -------------------------------------------------------

QString SavePrefix::savePath(const QString& path, const QString& extra) const
{
    QFileInfo info(path);
    QUrl savePath = QUrl::fromLocalFile(info.path());

    QString file = QString(extra);
    file.append(info.fileName());

    savePath.setPath(savePath.path() + '/' + file);
    return savePath.path();
}

// -------------------------------------------------------

QString SaveSuffix::savePath(const QString& path, const QString& extra) const
{
    QFileInfo info(path);
    QUrl savePath = QUrl::fromLocalFile(info.path());

    QString file = info.completeBaseName();
    file.append(extra);
    file.append(".");
    file.append(info.suffix());

    savePath.setPath(savePath.path() + '/' + file);
    return savePath.path();
}

// -------------------------------------------------------

QString SaveOverwrite::savePath(const QString& path, const QString& extra) const
{
    Q_UNUSED(extra);
    return path;
}

} // namespace KIPIRemoveRedEyesPlugin
