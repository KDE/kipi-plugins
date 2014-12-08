/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-06-01
 * @brief  c++ wrapper on ImageMagick Api
 *
 * @author Copyright (C) 2012      by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *         Copyright (C) 2012-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "magickiface.moc"

// Qt includes

#include <QImage>
#include <QColor>

// C++ includes

#include <cstdlib>
#include <cstring>
#include <climits>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

namespace KIPIPlugins
{

MagickImage::MagickImage()
{
    m_width  = 0;
    m_height = 0;
    m_image  = 0;
}

MagickImage::~MagickImage()
{
}

void MagickImage::setWidth(int width)
{
    m_width = width;
}

void MagickImage::setHeight(int height)
{
    m_height = height;
}

bool MagickImage::setImage(Image* const img)
{
    if (!img)
        return false;

    m_image = img;
    return true;
}

int MagickImage::getHeight() const
{
    return m_height;
}

int MagickImage::getWidth() const
{
    return m_width;
}

Image* MagickImage::getImage() const
{
    return m_image;
}

bool MagickImage::freeImage() const
{
    if (m_image)
        DestroyImage(m_image);

    return true;
}

// ----------------------------------------------------------------------------------

class MagickApi::Private
{
public:

    Private(MagickApi* const api)
    {
        filter = SCALE_FILTER_BEST;
        parent = api;
    }

    /// allocate a new image
    MagickImage* allocImage() const
    {
        MagickImage* img = 0;
        unsigned char pixels[4];

        ExceptionInfo exception;

        /* initialize the pixel data */
        memset(pixels, 0, sizeof(pixels));

        /* allocate a new image */
        if (!(img = new MagickImage()))
        {
            parent->Q_EMIT signalsAPIError("Out of memory");
            return 0;
        }

        img->setWidth(1);
        img->setHeight(1);

        GetExceptionInfo(&exception);

        if (!(img->setImage(ConstituteImage(1, 1, "RGB", CharPixel, pixels, &exception))))
        {
            parent->Q_EMIT signalsAPIError("ConstituteImage() failed");
            parent->freeImage(*img);
            return 0;
        }

        img->getImage()->compression = UndefinedCompression;
        img->getImage()->depth       = 16;
        DestroyExceptionInfo(&exception);

        return img;
    }

    void blendPixel(PixelPacket* const dst, PixelPacket* const src0, PixelPacket* const src1, float a)
    {
        dst->red   = blendPixelColor(src0->red,   src1->red,   a);
        dst->blue  = blendPixelColor(src0->blue,  src1->blue,  a);
        dst->green = blendPixelColor(src0->green, src1->green, a);
    }

    short unsigned int blendPixelColor(short unsigned int color0, short unsigned int color1, float a)
    {
        int d = (int) ((1 - (a)) * (color0) + (a) * (color1));

        if (d < 0)
            d = 0;

        if (d > USHRT_MAX)
            d = USHRT_MAX;

        return d;
    }

public:

    /// this is the temporary directory for storing files
    int        filter;
    MagickApi* parent;
};

MagickApi::MagickApi(const QString& path)
    : QObject(), d(new Private(this))
{
    // Iniialize ImageMagick lib
    MagickCoreGenesis(path.toLocal8Bit().data(), MagickFalse);
}

MagickApi::~MagickApi()
{
    MagickCoreTerminus();

    delete d;
}

int MagickApi::getFilter() const
{
    return d->filter;
}

MagickImage* MagickApi::loadQImage(const QImage& qimage)
{
    MagickImage* img      = 0;
    PixelPacket* img_data = 0;
    Image*       image    = 0;
    int x, y;

    img = d->allocImage();

    if (!img)
        return 0;

    if (!(image = ResizeImage(img->getImage(), qimage.width(), qimage.height(), SCALE_FILTER_FAST, 
          1.0, &img->getImage()->exception)))
    {
        Q_EMIT signalsAPIError("ResizeImage() failed\n");
        freeImage(*img);
        return 0;
    }

    DestroyImage(img->getImage());
    img->setImage(image);
    img->setWidth(img->getImage()->columns);
    img->setHeight(img->getImage()->rows);

    if (!(img_data = GetAuthenticPixels(img->getImage(), 0, 0, img->getWidth(), img->getHeight(), &img->getImage()->exception)))
    {
        Q_EMIT signalsAPIError("GetImagePixels() failed\n");
        freeImage(*img);
        return 0;
    }

    for (y = 0 ; y < img->getHeight() ; y++)
    {
        for (x = 0 ; x < img->getWidth() ; x++)
        {
            QColor rgb      = qimage.pixel(x, y);
            img_data->red   = rgb.red() * USHRT_MAX / UCHAR_MAX;
            img_data->green = rgb.green() * USHRT_MAX / UCHAR_MAX ;
            img_data->blue  = rgb.blue() * USHRT_MAX / UCHAR_MAX ;
            img_data++;
        }
    }

    SyncAuthenticPixels(img->getImage(), &img->getImage()->exception);

    return img;
}

MagickImage* MagickApi::loadImage(const QString& file)
{
    MagickImage*  img  = 0;
    ImageInfo*    info = 0;
    ExceptionInfo exception;

    img = d->allocImage();

    if (!img)
        return 0;

    GetExceptionInfo(&exception);

    if (!(info = CloneImageInfo((ImageInfo*) NULL)))
    {
        Q_EMIT signalsAPIError("CloneImageInfo() failed\n");
        freeImage(*img);
        return 0;
    }

    QString nfile = file;
    nfile.truncate(MaxTextExtent);

    strcpy(info->filename, nfile.toAscii());

    if (img->getImage())
        DestroyImage(img->getImage());

    if (!(img->setImage(ReadImage(info, &exception))))
    {
        Q_EMIT signalsAPIError("ReadImage(%s) failed\n");
        freeImage(*img);
        return 0;
    }

    img->setWidth(img->getImage()->columns);
    img->setHeight(img->getImage()->rows);
    DestroyImageInfo(info);
    DestroyExceptionInfo(&exception);

    return img;
}

MagickImage* MagickApi::loadStream(QFile& stream)
{
    if (stream.isOpen())
        stream.close();

    stream.open(QIODevice::ReadOnly);
    int fileHandle     = stream.handle();
    MagickImage*  img  = 0;
    ImageInfo*    info = 0;
    ExceptionInfo exception;

    img = d->allocImage();

    if (!img)
        return 0;

    GetExceptionInfo(&exception);

    if (!(info = CloneImageInfo((ImageInfo*) NULL)))
    {
        Q_EMIT signalsAPIError("CloneImageInfo() failed\n");
        freeImage(*img);
        return 0;
    }

    strcpy(info->filename, "");
    info->file = fdopen(fileHandle, "rb");

    if (img->getImage())
        DestroyImage(img->getImage());

    if (!(img->setImage(ReadImage(info,&exception))))
    {
        Q_EMIT signalsAPIError("ReadImage(%s) failed\n");
        freeImage(*img);
        return 0;
    }

    img->setWidth(img->getImage()->columns);
    img->setHeight(img->getImage()->rows);
    DestroyImageInfo(info);
    DestroyExceptionInfo(&exception);

    return img;
}

int MagickApi::saveToFile(const MagickImage& img, const QString& file)
{
    ImageInfo* info = 0;

    if (!(info = CloneImageInfo(NULL)))
    {
        Q_EMIT signalsAPIError("CloneImageInfo() failed\n");
        return -1;
    }

    QString nfile = file;
    nfile.truncate(MaxTextExtent);

    strcpy(info->filename, nfile.toAscii());
    strcpy(info->magick, "PPM");
    info->compression           = UndefinedCompression;
    info->depth                 = 8;
    img.getImage()->compression = UndefinedCompression;
    strcpy(img.getImage()->filename, nfile.toAscii());
    strcpy(img.getImage()->magick, "PPM");
    img.getImage()->depth       = 8;

    if (WriteImage(info, img.getImage()) != MagickTrue)
    {
        Q_EMIT signalsAPIError("WriteImage() failed\n");
        return -1;
    }

    return 1;
}

int MagickApi::saveToStream(const MagickImage& img, QFile& stream)
{
    if (stream.isOpen())
        stream.close();

    stream.open(QIODevice::WriteOnly);
    int fileHandle  = stream.handle();
    ImageInfo* info = 0;

    if (!(info = CloneImageInfo(NULL)))
    {
        Q_EMIT signalsAPIError("CloneImageInfo() failed\n");
        return -1;
    }

    info->file            = fdopen(fileHandle, "wb");
    strcpy(info->magick, "PPM");
    info->compression     = UndefinedCompression;
    info->depth           = 8;
    img.getImage()->compression = UndefinedCompression;
    strcpy(img.getImage()->filename, "");
    strcpy(img.getImage()->magick, "PPM");
    img.getImage()->depth = 8;

    if (WriteImage(info,img.getImage()) != MagickTrue)
    {
        Q_EMIT signalsAPIError("WriteImage() failed\n");
        return -1;
    }

    DestroyImageInfo(info);

    return 1;
}

MagickImage* MagickApi::createImage(const QString& color, int width, int height)
{
    MagickImage*  img   = 0;
    Image*        image = 0;
    ExceptionInfo exception;

    img = d->allocImage();

    if (!img)
        return 0;

    GetExceptionInfo(&exception);
    QueryColorDatabase(color.toAscii(), &img->getImage()->background_color, &img->getImage()->exception);
    SetImageBackgroundColor(img->getImage());

    if (!(image = ResizeImage(img->getImage(), width, height, SCALE_FILTER_FAST, 1.0, &exception)))
    {
        Q_EMIT signalsAPIError("ResizeImage() failed\n");
        freeImage(*img);
        return 0;
    }

    DestroyImage(img->getImage());
    img->setImage(image);
    img->setWidth(img->getImage()->columns);
    img->setHeight(img->getImage()->rows);
    DestroyExceptionInfo(&exception);

    if (img->getWidth() != width || img->getHeight() != height)
    {
        Q_EMIT signalsAPIError("frame doesn't have expected dimensions\n");
        freeImage(*img);
        return 0;
    }

    return img;
}

MagickImage* MagickApi::duplicateImage(const MagickImage& src)
{
    MagickImage*  dst = 0;
    ExceptionInfo exception;

    dst = d->allocImage();

    if (!dst)
        return 0;

    GetExceptionInfo(&exception);

    if (dst->getImage())
        DestroyImage(dst->getImage());

    if (!(dst->setImage(CloneImage(src.getImage(), 0, 0, (MagickBooleanType)1, &exception))))
    {
        Q_EMIT signalsAPIError("CloneImageInfo() failed\n");
        freeImage(*dst);
        return 0;
    }

    DestroyExceptionInfo(&exception);

    dst->setWidth(src.getWidth());
    dst->setHeight(src.getHeight());

    return dst;
}

bool MagickApi::freeImage(const MagickImage& img) const
{
    img.freeImage();
    delete(&img);

    return true;
}

int MagickApi::bitblitImage(MagickImage& dst, int dx, int dy, const MagickImage& src, int sx, int sy, int w, int h)
{
    Image* cropped = 0;
    Image* source  = src.getImage();
    ExceptionInfo exception;

    GetExceptionInfo(&exception);

    if (sx != 0 || sy != 0 || w != src.getWidth() || h != src.getHeight())
    {
        RectangleInfo geometry;
        geometry.x      = sx;
        geometry.y      = sy;
        geometry.width  = w;
        geometry.height = h;

        if (!(source = cropped = CropImage(src.getImage(), &geometry, &exception)))
        {
            Q_EMIT signalsAPIError("CropImage() failed\n");
            return -1;
        }
    }

    if (CompositeImage(dst.getImage(), SrcOverCompositeOp, source, dx, dy) != MagickTrue)
    {
        Q_EMIT signalsAPIError("CompositeImage() failed\n");
        return -1;
    }

    if (cropped)
        DestroyImage(cropped);

    DestroyExceptionInfo(&exception);

    return 1;
}

int MagickApi::blendImage(MagickImage& dst, const MagickImage& src0, const MagickImage& src1, float a)
{
    PixelPacket* src0_data = 0;
    PixelPacket* src1_data = 0;
    PixelPacket* dst_data  = 0;
    int x, y;

    /* check if the size matches */
    if (src0.getWidth() != src1.getWidth() || src0.getHeight() != src1.getHeight())
    {
        Q_EMIT signalsAPIError("scr0 size is not equal to src1");
        return -1;
    }

    if (dst.getWidth() != src0.getWidth() || dst.getHeight() != src0.getHeight())
    {
        Q_EMIT signalsAPIError("scr0 size is not equal to dst");
        return -1;
    }

    if (!(src0_data = GetAuthenticPixels(src0.getImage(), 0, 0, src0.getWidth(), src0.getHeight(), &src0.getImage()->exception)))
    {
        Q_EMIT signalsAPIError("GetImagePixels() failed\n");
        return -1;
    }

    if (!(src1_data = GetAuthenticPixels(src1.getImage(), 0, 0, src1.getWidth(), src1.getHeight(), &src1.getImage()->exception)))
    {
        Q_EMIT signalsAPIError("GetImagePixels() failed\n");
        return -1;
    }

    if (!(dst_data = GetAuthenticPixels(dst.getImage(), 0, 0, dst.getWidth(), dst.getHeight(), &dst.getImage()->exception)))
    {
        Q_EMIT signalsAPIError("GetImagePixels() failed\n");
        return -1;
    }

    /* do the blend */
    for (y = 0 ; y < dst.getWidth() ; y++)
    {
        /* do it every row */
        for (x = 0 ; x < dst.getHeight() ; x++)
        {
            d->blendPixel(dst_data, src0_data, src1_data, a);
            src0_data++;
            src1_data++;
            dst_data++;
        }
    }

    SyncAuthenticPixels(dst.getImage(), &dst.getImage()->exception);
    return 1;
}

MagickImage* MagickApi::borderImage(const MagickImage& simg, const QString& color, int bw, int bh)
{
    MagickImage* const img = createImage(color, simg.getWidth() + 2 * bw, simg.getHeight() + 2 * bh);

    if (!img)
        return 0;

    if (bitblitImage(*img, bw, bh, simg, 0, 0, simg.getWidth(), simg.getHeight()) != 1)
    {
        freeImage(*img);
        return 0;
    }

    return img;
}

MagickImage* MagickApi::geoscaleImage(const MagickImage& simg, int x, int y, int w, int h, int width, int height)
{
    MagickImage* img = 0;

    img = createImage("black", w, h);

    if (!img)
        return 0;

    /* copy the area out of the source image */
    if (bitblitImage(*img, 0, 0, simg, x, y, w, h) != 1)
    {
        freeImage(*img);
        return 0;
    }

    /* and scale it to correct output size */
    if (scaleImage(*img, width, height) != 1)
    {
        freeImage(*img);
        return 0;
    }

    return img;
}

bool MagickApi::overlayImage(MagickImage& dst, int dx, int dy, const MagickImage& src)
{
    return bitblitImage(dst, dx, dy, src, 0, 0, src.getWidth(), src.getHeight());
}

int MagickApi::scaleblitImage(MagickImage& dimg, int dx, int dy, int dw, int dh, const MagickImage& simg, 
                              int sx, int sy, int sw, int sh)
{
    /* scale the source image */
    MagickImage* const img = geoscaleImage(simg, sx, sy, sw, sh, dw, dh);

    if (!img)
        return -1;

    if (bitblitImage(dimg, dx, dy, *img, 0, 0, dw, dh) != 1)
    {
        freeImage(*img);
        return -1;
    }

    if (!freeImage(*img))
        return -1;

    return 1;
}

int MagickApi::scaleImage(MagickImage& img, int width, int height)
{
    Image*        image = 0;
    ExceptionInfo exception;

    if (img.getWidth() != width || img.getHeight() != height)
    {
        GetExceptionInfo(&exception);

        if (!(image = ResizeImage(img.getImage(), width, height,(FilterTypes)d->filter, 1.0, &exception)))
        {
            Q_EMIT signalsAPIError("ResizeImage() failed\n");
            return -1;
        }

        DestroyImage(img.getImage());
        img.setImage(image);
        img.setWidth(img.getImage()->columns);
        img.setHeight(img.getImage()->rows);
        DestroyExceptionInfo(&exception);

        if (img.getWidth() != width || img.getHeight() != height)
        {
            Q_EMIT signalsAPIError("actual size is not equal to the expected size\n");
            return -1;
        }
    }

    return 1;
}

bool MagickApi::displayImage(MagickImage& img)
{
#ifdef WINDOWS
    // ImageMagick's display doesn't work on windows
    return true;
#endif

    ImageInfo* info = 0;

    if (!(info = CloneImageInfo((ImageInfo*) NULL)))
    {
        Q_EMIT signalsAPIError("CloneImageInfo() failed\n");
        return false;
    }

    MagickBooleanType done = DisplayImages(info, img.getImage());

    if (done == MagickTrue)
        return true;

    DestroyImageInfo(info);

    return false;
}

} // namespace KIPIPlugins
