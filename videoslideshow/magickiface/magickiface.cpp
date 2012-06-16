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

#include "magickiface.moc"

// C++ includes

#include <cstdlib>
#include <cstring>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

MagickImage::MagickImage()
{
    m_width  = 0;
    m_height = 0;
    m_image  = 0;
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
    if(!img)
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
    if(m_image)
        DestroyImage(m_image);

    return true;
}

// ----------------------------------------------------------------------------------

MagickApi::MagickApi()
    : QObject()
{
    /* Iniialize ImageMagick lib */
    MagickCoreGenesis(m_cwd = GetCurrentDir(NULL, 0), MagickFalse);
    m_filter                = SCALE_FILTER_FAST;
}

MagickApi::~MagickApi()
{
    if (m_cwd)
    {
        free(m_cwd);
        m_cwd = NULL;
    }

    MagickCoreTerminus();
}

int MagickApi::getFilter() const
{
    return m_filter;
}

MagickImage* MagickApi::allocImage()
{
    MagickImage*  img = 0;
    unsigned char pixels[4];

    ExceptionInfo exception;

    /* initialize the pixel data */
    memset(pixels, 0, sizeof(pixels));

    /* allocate a new image */
    if (!(img = (MagickImage*) malloc(sizeof(MagickImage))))
    {
        emit signalsAPIError("Out of memory");
        return 0;
    }

    memset(img, 0, sizeof(MagickImage));
    img->setWidth(1);
    img->setHeight(1);

    GetExceptionInfo(&exception);
    if (!(img->setImage(ConstituteImage(1, 1, "RGB", CharPixel, pixels, &exception))))
    {
        emit signalsAPIError("ConstituteImage() failed");
        return 0;
    }
    img->getImage()->compression = UndefinedCompression;
    img->getImage()->depth       = 8;
    DestroyExceptionInfo(&exception);

    return img;
}

MagickImage* MagickApi::loadImage(const QString& file)
{
    MagickImage*  img  = 0;
    ImageInfo*    info = 0;
    ExceptionInfo exception;

    img = allocImage();
    if(!img)
        return 0;

    GetExceptionInfo(&exception);
    if (!(info = CloneImageInfo((ImageInfo*) NULL)))
    {
        emit signalsAPIError("CloneImageInfo() failed\n");
        return 0;
    }

    strcpy(info->filename,file.toAscii());
    if (img->getImage())
        DestroyImage(img->getImage());

    if (!(img->setImage(ReadImage(info, &exception))))
    {
        emit signalsAPIError("ReadImage(%s) failed\n");
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
    if(stream.isOpen())
        stream.close();

    stream.open(QIODevice::ReadOnly);
    int fileHandle = stream.handle();

    MagickImage*  img  = 0;
    ImageInfo*    info = 0;
    ExceptionInfo exception;

    img = allocImage();
    if (!img)
        return 0;

    GetExceptionInfo(&exception);
    if (!(info = CloneImageInfo((ImageInfo*) NULL)))
    {
        emit signalsAPIError("CloneImageInfo() failed\n");
        return 0;
    }

    strcpy(info->filename,"");
    info->file = fdopen(fileHandle,"rb");

    if (img->getImage())
        DestroyImage(img->getImage());

    if (!(img->setImage(ReadImage(info,&exception))))
    {
        emit signalsAPIError("ReadImage(%s) failed\n");
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
        emit signalsAPIError("CloneImageInfo() failed\n");
        return -1;
    }

    strcpy(info->filename, file.toAscii());
    strcpy(info->magick,"PPM");
    info->compression           = UndefinedCompression;
    info->depth                 = 8;
    img.getImage()->compression = UndefinedCompression;
    strcpy(img.getImage()->filename,file.toAscii());
    strcpy(img.getImage()->magick,"PPM");
    img.getImage()->depth       = 8;

    if (WriteImage(info,img.getImage()) != MagickTrue)
    {
        emit signalsAPIError("WriteImage() failed\n");
        return -1;
    }

    return 1;
}

int MagickApi::saveToStream(const MagickImage& img, QFile& stream)
{
    if(stream.isOpen())
        stream.close();

    stream.open(QIODevice::WriteOnly);
    int fileHandle = stream.handle();

    ImageInfo* info = 0;

    if (!(info = CloneImageInfo(NULL)))
    {
        emit signalsAPIError("CloneImageInfo() failed\n");
        return -1;
    }

    info->file            = fdopen(fileHandle,"wb");
    strcpy(info->magick,"PPM");
    info->compression     = UndefinedCompression;
    info->depth           = 8;
    img.getImage()->compression = UndefinedCompression;
    strcpy(img.getImage()->filename,"");
    strcpy(img.getImage()->magick,"PPM");
    img.getImage()->depth = 8;

    if (WriteImage(info,img.getImage()) != MagickTrue)
    {
        emit signalsAPIError("WriteImage() failed\n");
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

    img = allocImage();
    if (!img)
        return 0;

    GetExceptionInfo(&exception);
    QueryColorDatabase(color.toAscii(), &img->getImage()->background_color, &img->getImage()->exception);
    SetImageBackgroundColor(img->getImage());
    
    if (!(image = ResizeImage(img->getImage(), width, height, SCALE_FILTER_FAST, 1.0, &exception)))
    {
        emit signalsAPIError("ResizeImage() failed\n");
        return 0;
    }

    DestroyImage(img->getImage());
    img->setImage(image);
    img->setWidth(img->getImage()->columns);
    img->setHeight(img->getImage()->rows);
    DestroyExceptionInfo(&exception);

    if (img->getWidth() != width || img->getWidth() != height)
    {
        emit signalsAPIError("frame doesn't have expected dimensions\n");
        return 0;
    }

    return img;
}

MagickImage* MagickApi::duplicateImage(const MagickImage& src)
{
    MagickImage*  dst = 0;
    ExceptionInfo exception;

    dst = allocImage();
    if (!dst)
        return 0;

    GetExceptionInfo(&exception);
    if (dst->getImage())
        DestroyImage(dst->getImage());

    if (!(dst->setImage(CloneImage(src.getImage(),0,0,(MagickBooleanType)1,&exception))))
    {
        emit signalsAPIError("CloneImageInfo() failed\n");
        return 0;
    }

    DestroyExceptionInfo(&exception);

    dst->setWidth( src.getWidth());
    dst->setHeight( src.getHeight());

    return dst;
}

bool MagickApi::freeImage(const MagickImage& img) const
{
    img.freeImage();
    delete(&img);

    return true;
}

bool MagickApi::bitblitImage(MagickImage& dst, int dx, int dy, const MagickImage& src, int sx, int sy, int w, int h)
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
            emit signalsAPIError("CropImage() failed\n");
            return 0;
        }
    }

    if (CompositeImage(dst.getImage(), SrcOverCompositeOp, source, dx, dy) != MagickTrue)
    {
        emit signalsAPIError("CompositeImage() failed\n");
        return -1;
    }

    if (cropped)
        DestroyImage(cropped);

    DestroyExceptionInfo(&exception);

    return 1;
}

short unsigned int blendPixelColor(short unsigned int color0, short unsigned int color1, float a)
{
    int d = (int) ((1 - (a)) * (color0) + (a) * (color1));

    if (d < 0)
        d = 0;

    if (d > 255)
        d = 255;

    return d;
}

void blendPixel(PixelPacket* dst, PixelPacket* src0, PixelPacket* src1, float a)
{
    dst->red   = blendPixelColor(src0->red,   src1->red,   a);
    dst->blue  = blendPixelColor(src0->blue,  src1->blue,  a);
    dst->green = blendPixelColor(src0->green, src1->green, a);
}

bool MagickApi::blendImage(MagickImage& dst, const MagickImage& src0, const MagickImage& src1, float a)
{
    PixelPacket* src0_data = 0;
    PixelPacket* src1_data = 0;
    PixelPacket* dst_data  = 0;
    int x, y;

    /* check if the size matches */
    if (src0.getWidth() != src1.getWidth() || src0.getHeight() != src1.getHeight())
    {
        emit signalsAPIError("scr0 size is not equal to src1");
        return -1;
    }

    if (dst.getWidth() != src0.getWidth() || dst.getHeight() != src0.getHeight())
    {
        emit signalsAPIError("scr0 size is not equal to dst");
        return -1;
    }

    if (!(src0_data = GetAuthenticPixels(src0.getImage(), 0, 0, src0.getWidth(), src0.getHeight(), &src0.getImage()->exception)))
    {
        emit signalsAPIError("GetImagePixels() failed\n");
        return -1;
    }

    if (!(src1_data = GetAuthenticPixels(src1.getImage(), 0, 0, src1.getWidth(), src1.getHeight(), &src1.getImage()->exception)))
    {
        emit signalsAPIError("GetImagePixels() failed\n");
        return -1;
    }

    if (!(dst_data = GetAuthenticPixels(dst.getImage(), 0, 0, dst.getWidth(), dst.getHeight(), &dst.getImage()->exception)))
    {
        emit signalsAPIError("GetImagePixels() failed\n");
        return -1;
    }

    /* do the blend */
    for (y = 0 ; y < dst.getWidth() ; y++)
    {
        /* do it every row */
        for (x = 0 ; x < dst.getHeight() ; x++)
        {
            blendPixel(dst_data, src0_data, src1_data, a);
            src0_data++;
            src1_data++;
            dst_data++;
        }
    }

    SetImageBackgroundColor(dst.getImage());
    return 1;
}

MagickImage* MagickApi::borderImage(const MagickImage& simg, const QString& color, int bw, int bh)
{
    MagickImage* img = 0;

    img = createImage(color, simg.getWidth() + 2 * bw, simg.getWidth() + 2 * bh);
    if (!img)
        return 0;

    if(!bitblitImage(*img, bw, bh, simg, 0, 0, simg.getWidth(), simg.getWidth()))
        return 0;

    return img;
}

MagickImage* MagickApi::geoscaleImage(const MagickImage& simg, int x, int y, int w, int h, int width, int height)
{
    MagickImage* img = 0;

    img = createImage("black", w, h);
    if (!img)
        return 0;

    /* copy the area out of the source image */
    if(!bitblitImage(*img, 0, 0, simg, x, y, w, h))
        return 0;

    /* and scale it to correct output size */
    if(!scaleImage(*img, width, height))
        return 0;

    return img;
}

bool MagickApi::overlayImage(MagickImage& dst, int dx, int dy, const MagickImage& src)
{
    return bitblitImage(dst, dx, dy, src, 0, 0, src.getWidth(), src.getHeight());
}

bool MagickApi::scaleblitImage(MagickImage& dimg, int dx, int dy, int dw, int dh, const MagickImage& simg, int sx, int sy, int sw, int sh)
{
    MagickImage* img = 0;

    /* scale the source image */
    img = geoscaleImage(simg, sx, sy, sw, sh, dw, dh);
    if (!img)
        return -1;

    if(!bitblitImage(dimg, dx, dy, *img, 0, 0, dw, dh))
        return -1;

    if(!freeImage(*img))
        return -1;

    return 1;
}

bool MagickApi::scaleImage(MagickImage& img, int width, int height)
{
    Image*        image = 0;
    ExceptionInfo exception;

    if (img.getWidth() != width || img.getHeight() != height)
    {
        GetExceptionInfo(&exception);
        if (!(image = ResizeImage(img.getImage(), width, height,(FilterTypes)m_filter, 1.0, &exception)))
        {
            emit signalsAPIError("ResizeImage() failed\n");
            return -1;
        }

        DestroyImage(img.getImage());
        img.setImage(image);
        img.setWidth(img.getImage()->columns);
        img.setHeight(img.getImage()->rows);
        DestroyExceptionInfo(&exception);

        if (img.getWidth() != width || img.getHeight() != height)
        {
            emit signalsAPIError("actual size is not equal to the expected size\n");
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
        emit signalsAPIError("CloneImageInfo() failed\n");
        return 0;
    }

    MagickBooleanType done = DisplayImages(info, img.getImage());
    if (done == MagickTrue)
        return true;

    DestroyImageInfo(info);

    return false;
}
