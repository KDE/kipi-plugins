/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : a plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#ifndef JALBUM_H
#define JALBUM_H

// Qt includes

#include <QString>

// KDE includes

#include <kurl.h>

namespace KIPIJAlbumExportPlugin
{

class JAlbum
{

public:

    JAlbum();
    ~JAlbum();

    KUrl albumPath() const;
    KUrl jarPath()   const;

    void setPath(const QString& albumPath);
    void setJar(const QString& jar);

    void save();

    static bool createDir(const QString& dirName);

private:

    void load();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIJAlbumExportPlugin

#endif /* JALBUM_H */
