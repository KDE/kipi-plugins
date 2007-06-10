/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : misc utils to used in batch process
 * 
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef UTILS_H
#define UTILS_H

class QString;

namespace KIPIJPEGLossLessPlugin
{

class Utils
{

public:

    /** Test if a file is a JPEG file.
    */
    static bool isJPEG(const QString& file);

    /** Test if a file is a RAW file supported by dcraw.
    */
    static bool isRAW(const QString& file);

    /** POSIX Compliant File Copy and Move -
        Can't use KIO based operations as we need to use these in a thread
    */
    static bool CopyFile(const QString& src, const QString& dst);
    static bool MoveFile(const QString& src, const QString& dst);

    /** Thread-safe recursive dir deletion.
    */
    static bool deleteDir(const QString& dirPath);
};

}  // NameSpace KIPIJPEGLossLessPlugin

#endif /* UTILS_H */
