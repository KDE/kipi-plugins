/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2003-10-14
 * Description : batch image flip
 * 
 * Copyright 2003-2006 by Gilles Caulier <caulier dot gilles at kdemail dot net>
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

#ifndef IMAGEFLIP_H
#define IMAGEFLIP_H

// Local includes.

#include "actions.h"

// Qt includes.

#include <qobject.h>

class QString;

class KProcess;

namespace KIPIJPEGLossLessPlugin
{

class ImageFlip : public QObject
{
    Q_OBJECT

public:

    ImageFlip();
    ~ImageFlip();

    bool flip(const QString& src, FlipAction action, const QString& TmpFolder, QString& err);

private slots:

    void slotReadStderr(KProcess*, char*, int);

private:

    bool flipJPEG(const QString& src, const QString& dest, FlipAction action, QString& err);
    
    bool flipImageMagick(const QString& src, const QString& dest, FlipAction action, QString& err);


private:

    QString m_stdErr;
};

}  // NameSpace KIPIJPEGLossLessPlugin

#endif /* IMAGEFLIP_H */
