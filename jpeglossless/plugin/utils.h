/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-12-03
 * Description : misc utils to used in batch process
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QObject>
#include <QString>

namespace KIPIJPEGLossLessPlugin
{

class Utils : public QObject
{
    Q_OBJECT

public:

    Utils(QObject* const parent);
    ~Utils();

    bool updateMetadataImageMagick(const QString& src, QString& err);

public: // Static methods.

    /** Test if a file is a JPEG file.
    */
    static bool isJPEG(const QString& file);

    /** POSIX Compliant File Copy and Move -
        Can't use KIO based operations as we need to use these in a thread
    */

    static bool copyOneFile(const QString& src, const QString& dst);
    static bool moveOneFile(const QString& src, const QString& dst);

    /** Thread-safe recursive dir deletion.
    */

    static bool deleteDir(const QString& dirPath);

private:

    QString m_stdErr;
};

}  // namespace KIPIJPEGLossLessPlugin

#endif /* UTILS_H */
