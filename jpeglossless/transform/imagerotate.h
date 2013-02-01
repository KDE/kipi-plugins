/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-14
 * Description : batch image rotation
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

#ifndef IMAGEROTATE_H
#define IMAGEROTATE_H

// Qt includes

#include <QObject>

// KDE includes

#include <ktemporaryfile.h>

// Local includes

#include "actions.h"

namespace KIPIJPEGLossLessPlugin
{

class ImageRotate : public QObject
{
    Q_OBJECT

public:

    ImageRotate();
    ~ImageRotate();

    bool rotate(const QString& src, RotateAction angle, QString& err);

private:

    bool rotateJPEG(const QString& src, const QString& dest, RotateAction angle, QString& err);
    bool rotateImageMagick(const QString& src, const QString& dest, RotateAction angle, QString& err);

private:

    QString        m_stdErr;
    KTemporaryFile m_tmpFile;
};

}  // namespace KIPIJPEGLossLessPlugin

#endif /* IMAGEROTATE_H */
