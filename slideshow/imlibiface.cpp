/* ============================================================
 * File  : imlibiface.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-16
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

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

#include <qwidget.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpixmap.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Imlib.h>

#include "imlibiface.h"

// ---------------------------------------------------------------

class ImImageSSPrivate
{
public:
    
    ImlibData   *idata;
    ImlibImage  *image;
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

    QWidget   *parent;
    ImlibData *idata;
    Display   *display;
    Window     win;
    GC         gc;
};

// ---------------------------------------------------------------

ImImageSS::ImImageSS(ImlibIface *imIface, const QString& file)
{
    imIface_ = imIface;
    
    d = new ImImageSSPrivate;
    d->filename = file;
    d->idata = (ImlibData*) imIface->imlibData();

    d->valid      = false;
    d->image      = 0;
    d->width      = 0;
    d->height     = 0;
    d->origWidth  = 0;
    d->origHeight = 0;
    d->fitWidth   = 0;
    d->fitHeight  = 0;

    d->image = Imlib_load_image(d->idata,
                                QFile::encodeName(file).data());
    if (d->image) {
        d->valid = true;
        d->origWidth  = d->image->rgb_width;
        d->origHeight = d->image->rgb_height;
        d->width      = d->origWidth;
        d->height     = d->origHeight;
    }
}

ImImageSS::~ImImageSS()
{
    if (d->image)
        Imlib_kill_image(d->idata, d->image);
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
    if (!d->valid) return;


    Pixmap pixmap;
    
    Imlib_render(d->idata, d->image, d->width, d->height);
    pixmap = Imlib_move_image(d->idata, d->image);

    if (pixmap) {

        // Center the image
        
        int cx = d->fitWidth/2;
        int cy = d->fitHeight/2;

        int x = cx - d->width/2;
        int y = cy - d->height/2;
        
        XSetGraphicsExposures(imIface_->d->display,
                              imIface_->d->gc, False);
        XCopyArea(imIface_->d->display,
                  pixmap,
                  d->qpixmap.handle(),
                  imIface_->d->gc,
                  0, 0, d->width, d->height, x, y);
        Imlib_free_pixmap(d->idata, pixmap);
    }
    else {
        qWarning("Failed to convert to pixmap");
        d->valid = false;
    }
}

QPixmap* ImImageSS::qpixmap()
{
    return &(d->qpixmap);    
}

// ---------------------------------------------------------------

ImlibIface::ImlibIface(QWidget *parent)
{
    d = new ImlibIfacePrivate;

    d->parent  = parent;
    d->display = parent->x11Display();
    d->win     = parent->handle();
    d->gc = XCreateGC(parent->x11Display(),
                      RootWindow(parent->x11Display(),
                                 parent->x11Screen()),
                      0, 0);
    ImlibInitParams par;
    par.flags = ( PARAMS_REMAP |
                  PARAMS_FASTRENDER | PARAMS_HIQUALITY |
                  PARAMS_DITHER |
                  PARAMS_IMAGECACHESIZE | PARAMS_PIXMAPCACHESIZE );
    par.remap           = 1;
    par.fastrender      = 1;
    par.hiquality       = 1;
    par.dither          = 1;
    uint maxcache       = 10240;
    par.imagecachesize  = maxcache * 1024;
    par.pixmapcachesize = maxcache * 1024;
    d->idata = Imlib_init_with_params(d->display, &par );
}

ImlibIface::~ImlibIface()
{
    XFreeGC(d->display, d->gc);
    delete d;
}

void ImlibIface::paint(ImImageSS *image, int sx, int sy,
                       int dx, int dy, int dw, int dh)
{
    if (!image || !image->valid()) return;

    //     bitBlt(d->parent, sx, sy, image->qpixmap(),
    //            dx, dy, dw, dh, Qt::CopyROP, true);
    
    XSetGraphicsExposures(d->display, d->gc, False);
    XCopyArea(d->display,
              image->qpixmap()->handle(),
              d->win,
              d->gc, dx, dy, dw, dh, sx, sy);
}

void* ImlibIface::imlibData()
{
    return d->idata;
}

