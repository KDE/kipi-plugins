/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-14
 * Description : batch image flip
 *
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2003-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGEFLIP_H
#define IMAGEFLIP_H

// Local includes

#include "actions.h"

// Qt includes

#include <QObject>

// KDE includes

#include <ktemporaryfile.h>

class QString;

namespace KIPIJPEGLossLessPlugin
{

class ImageFlip : public QObject
{
    Q_OBJECT

public:

    ImageFlip();
    ~ImageFlip();

    bool flip(const QString& src, FlipAction action, QString& err);

private:

    bool flipJPEG(const QString& src, const QString& dest, FlipAction action, QString& err);
    bool flipImageMagick(const QString& src, const QString& dest, FlipAction action, QString& err);

private:

    QString        m_stdErr;
    KTemporaryFile m_tmpFile;
};

}  // namespace KIPIJPEGLossLessPlugin

#endif /* IMAGEFLIP_H */
