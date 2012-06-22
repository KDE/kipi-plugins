/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-06-01
 * @brief  c++ wrapper on ImageMagick Api
 *
 * @author Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *         Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef MAGICK_API_H
#define MAGICK_API_H

// Qt includes

#include <QObject>
#include <QFile>
#include <QString>

// ImageMagick includes

// To prevent compilation warnings about undefined QuantumDepth
#define QuantumDepth  MAGICKCORE_QUANTUM_DEPTH

#include <magick/api.h>

#define SCALE_FILTER_BEST GaussianFilter
#define SCALE_FILTER_FAST PointFilter

namespace KIPIPlugins
{

/// wraper over ImageMagick Image container to use in c++ code
class MagickImage
{

public:

    MagickImage();
    ~MagickImage();

    void setWidth(int width);
    void setHeight(int height);
    bool setImage(Image* const img);

    int    getWidth()  const;
    int    getHeight() const;
    Image* getImage()  const;
    bool   freeImage() const;

private:

    int    m_width;
    int    m_height;
    Image* m_image;
};

// ------------------------------------------------------------------------------

/// c++ wrapper over ImageMagic API written in "api.h"
class MagickApi : public QObject
{
    Q_OBJECT

public:

    MagickApi();
    ~MagickApi();

    MagickImage* loadImage(const QString& file);
    MagickImage* loadStream(QFile& stream);

    int saveToFile(const MagickImage& img, const QString& file);
    int saveToStream(const MagickImage& img, QFile& stream);

    MagickImage* createImage(const QString& color, int width, int height);
    MagickImage* duplicateImage(const MagickImage& src);

    bool freeImage(const MagickImage& img) const;

    bool blendImage(MagickImage& dst, const MagickImage& src0, const MagickImage& src1, float a);
    bool overlayImage(MagickImage& dst, int dx, int dy, const MagickImage& src);
    bool bitblitImage(MagickImage& dst, int dx, int dy, const MagickImage& src, int sx, int sy, int w, int h);
    bool scaleImage(MagickImage& img, int width, int height);
    bool scaleblitImage(MagickImage& dimg, int dx, int dy, int dw, int dh, const MagickImage& simg, int sx, int sy, int sw, int sh);

    MagickImage* geoscaleImage(const MagickImage& img, int x, int y, int w, int h, int width, int height);
    MagickImage* borderImage(const MagickImage& img, const QString& rgbcolor, int bw, int bh);

    /// used to display image
    bool displayImage(MagickImage& img);

    /// returns filter used during resize of image
    int getFilter() const;

Q_SIGNALS:

    void signalsAPIError(const QString& error);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif // MAGICK_API_H
