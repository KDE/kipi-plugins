/* ============================================================
 * File  : resizeimage.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2004-02-26
 * Description : Multithreaded image resize process
 *
 * Copyright 2004 by Gilles Caulier

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef RESIZE_IMAGE_H
#define RESIZE_IMAGE_H

// Include files for Qt

#include <qthread.h>
#include <qstring.h>

class QObject;

namespace KIPIImagesGalleryPlugin
{

class ResizeImage : public QThread
{
public:
    ResizeImage(QObject *parent,
                const QString Path, const QString Directory, const QString ImageFormat,
                const QString ImageNameFormat, int *Width, int *Height, int SizeFactor,
                bool ColorDepthChange, int ColorDepthValue, bool CompressionSet,
                int ImageCompression, bool *threadDone, bool *useBrokenImage);
    ~ResizeImage();

protected:
    void run();

private:
    QObject  *parent_;

    QString   Path_;
    QString   Directory_;
    QString   ImageFormat_;
    QString   ImageNameFormat_;

    int      *Width_;
    int      *Height_;
    int       SizeFactor_;
    int       ColorDepthValue_;
    int       ImageCompression_;

    bool      ColorDepthChange_;
    bool      CompressionSet_;
    bool     *threadDone_;
    bool     *useBrokenImage_;
};

}  // NameSpace KIPIImagesGalleryPlugin

#endif // RESIZE_IMAGE_H
