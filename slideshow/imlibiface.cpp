/* ============================================================
 * File  : imlibiface.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-16
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

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

#include <qwidget.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpixmap.h>

#include <X11/Xlib.h>
#include <Imlib2.h>

#include "imlibiface.h"

namespace KIPISlideShowPlugin
{

// ---------------------------------------------------------------

class ImImageSSPrivate
{
public:
    
    Imlib_Image   image;
    int          width;
    int          height;
    int          origWidth;
    int          origHeight;
    int          fitWidth;
    int          fitHeight;
    bool         valid;
    QPixmap      qpixmap;
    QString      filename;
};

// ---------------------------------------------------------------

class ImlibIfacePrivate {

public:

    QWidget       *parent;
    Imlib_Context  context;
};

// ---------------------------------------------------------------

ImImageSS::ImImageSS(ImlibIface *imIface, const QString& file, int angle)
{
    imIface_ = imIface;
    
    d = new ImImageSSPrivate;
    d->filename = file;

    d->valid      = false;
    d->image      = 0;
    d->width      = 0;
    d->height     = 0;
    d->origWidth  = 0;
    d->origHeight = 0;
    d->fitWidth   = 0;
    d->fitHeight  = 0;

    imlib_context_push(imIface_->d->context);
    
    d->image = imlib_load_image(QFile::encodeName(file).data());
    if (d->image) {
        imlib_context_set_image(d->image);
        d->valid = true;

        switch(angle)
        {
        case(90):
            imlib_image_orientate(1);
            break;
        case(180):
            imlib_image_orientate(2);
            break;
        case(270):
            imlib_image_orientate(3);
            break;
        default:
            break;
        }

        d->origWidth  = imlib_image_get_width();
        d->origHeight = imlib_image_get_height();
        d->width      = d->origWidth;
        d->height     = d->origHeight;
        
    }

    imlib_context_pop();
}

ImImageSS::~ImImageSS()
{
    if (d->image) {
        imlib_context_push(imIface_->d->context);
        imlib_context_set_image(d->image);
        imlib_free_image();
        imlib_context_pop();
    }
    delete d;
}

QString ImImageSS::filename()
{
    QFileInfo fi(d->filename);
    return fi.fileName();
}

bool ImImageSS::valid()
{
    return d->valid;    
}

void ImImageSS::fitSize(int width, int height)
{
    d->fitWidth  = width;
    d->fitHeight = height;
    
    float zoom;
    if (d->origWidth  < width &&
        d->origHeight < height)
        zoom = 1.0;
    else
        zoom = QMIN((float)width/(float)d->origWidth,
                    (float)height/(float)d->origHeight);

    d->width  = int(zoom * d->origWidth);
    d->height = int(zoom * d->origHeight);

    d->qpixmap.resize(width, height);
    d->qpixmap.fill(Qt::black);
}


void ImImageSS::render()
{
    if (!d->valid)
        return;

    // Center the image
        
    int cx = d->fitWidth/2;
    int cy = d->fitHeight/2;
    
    int x = cx - d->width/2;
    int y = cy - d->height/2;

    imlib_context_push(imIface_->d->context);

    imlib_context_set_image(d->image);
    imlib_context_set_drawable(d->qpixmap.handle());

    imlib_render_image_on_drawable_at_size(x, y, d->width, d->height);

    imlib_context_pop();
}

QPixmap* ImImageSS::qpixmap()
{
    return &(d->qpixmap);    
}

// ---------------------------------------------------------------

ImlibIface::ImlibIface(QWidget *parent)
{
    d = new ImlibIfacePrivate;

    d->context = imlib_context_new();
    imlib_context_push(d->context);

    Display *display = parent->x11Display();
    Visual  *vis     = DefaultVisual(display, DefaultScreen(display));
    Colormap cm      = DefaultColormap(display, DefaultScreen(display));

    d->parent  = parent;
    
    // 10 MB of cache
    imlib_set_cache_size(10 * 1024 * 1024);

    // set the maximum number of colors to allocate for 8bpp
    // and less to 128 
    imlib_set_color_usage(128);

    // dither for depths < 24bpp 
    imlib_context_set_dither(1);

    // set the display , visual, colormap we are using 
    imlib_context_set_display(display);
    imlib_context_set_visual(vis);
    imlib_context_set_colormap(cm);

    // smooth scaling
    imlib_context_set_anti_alias(1);

    imlib_context_pop();
}

ImlibIface::~ImlibIface()
{
    imlib_context_free(d->context);
    delete d;
}

}  // NameSpace KIPISlideShowPlugin

