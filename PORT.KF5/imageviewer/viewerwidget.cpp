/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
 * Copyright (C) 2008-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "viewerwidget.moc"

// Qt includes

#include <QDesktopWidget>
#include <QPointer>

// KDE includes

#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

// Libkipi includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "timer.h"
#include "texture.h"
#include "helpdialog.h"

#ifndef GL_TEXTURE_RECTANGLE_ARB
#define GL_TEXTURE_RECTANGLE_ARB   0x84F5
#endif

#ifndef GL_TEXTURE_RECTANGLE_NV
#define GL_TEXTURE_RECTANGLE_NV    0x84F5
#endif

namespace KIPIViewerPlugin
{

class ViewerWidget::Private
{
public:

    struct Cache
    {
        int      file_index;
        Texture* texture;
    };

    enum WheelAction
    {
        zoomImage,
        changeImage
    };

public:

    Private()
    {
        texture    = 0;
        firstImage = true;

        // index of picture to be displayed
        file_idx   = 0;

        // define zoomfactors for one zoom step
        zoomfactor_scrollwheel = 1.1F;
        zoomfactor_mousemove   = 1.03F;
        zoomfactor_keyboard    = 1.05F;

        // load cursors for zooming and panning
        zoomCursor = QCursor(QPixmap(KStandardDirs::locate("data", "kipiplugin_imageviewer/pics/zoom.png")));
        moveCursor = QCursor(QPixmap(KStandardDirs::locate("data", "kipiplugin_imageviewer/pics/hand.png")));

        // get path of nullImage in case QImage can't load the image
        nullImage  = KStandardDirs::locate( "data", "kipiplugin_imageviewer/pics/nullImage.png" );

        // while zooming is performed, the image is downsampled to d->zoomsize. This seems to
        // be the optimal way for a PentiumM 1.4G, Nvidia FX5200. For a faster setup, this might
        // not be necessary anymore
        zoomsize   = QSize(1024, 768);

        iface                  = 0;
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }

        for (int i = 0; i < CACHESIZE ; i++)
        {
            cache[i].file_index = 0;
            cache[i].texture    = 0;
        }

        ratio_view_x  = 0.0F;
        ratio_view_y  = 0.0F;
        delta         = 0.0F;
        vertex_height = 0.0F;
        vertex_width  = 0.0F;
        vertex_left   = 0.0F;
        vertex_top    = 0.0F;
        vertex_right  = 0.0F;
        vertex_bottom = 0.0F;
        wheelAction   = zoomImage;
        screen_width  = 0;
    }

    GLuint           tex[3];
    QStringList      files;
    unsigned int     file_idx;
    Cache            cache[CACHESIZE];
    Texture*         texture;
    float            ratio_view_y, ratio_view_x, delta;
    float            vertex_height, vertex_width, vertex_left, vertex_top, vertex_right, vertex_bottom;
    QPoint           startdrag, previous_pos;
    WheelAction      wheelAction;
    bool             firstImage;
    QSize            zoomsize;
    QTimer           timerMouseMove;
    QCursor          moveCursor, zoomCursor;
    float            zoomfactor_scrollwheel, zoomfactor_mousemove, zoomfactor_keyboard;
    QString          nullImage;
    int              screen_width;
    Interface*       iface;
};

ViewerWidget::ViewerWidget()
    : d(new Private)
{
    ImageCollection selection = d->iface->currentSelection();
    ImageCollection album     = d->iface->currentAlbum();

    KUrl::List myfiles; //pics which are displayed in imageviewer
    QString selectedImage; //selected pic in hostapp

    int foundNumber = 0;

    //determine screen size for isReallyFullScreen
    QDesktopWidget dw;
    d->screen_width = dw.screenGeometry(this).width();

    if ( selection.images().count()==0 )
    {
        kDebug() << "no image selected, load entire album" ;
        myfiles = album.images();
    }
    else if ( selection.images().count()==1 )
    {
        kDebug() << "one image selected, load entire album and start with selected image" ;
        selectedImage = selection.images().first().path();
        myfiles       = album.images();
    }
    else if ( selection.images().count()>1 )
    {
        kDebug() << "load " << selection.images().count() << " selected images" ;
        myfiles = selection.images();
    }

    // populate QStringList::d->files
    for(KUrl::List::Iterator it = myfiles.begin(); it != myfiles.end(); ++it)
    {
        // find selected image in album in order to determine the first displayed image
        // in case one image was selected and the entire album was loaded
        it->cleanPath();
        QString s = (*it).path();

        if ( s == selectedImage )
        {
            kDebug() << "selected img  " << selectedImage << " has idx=" << foundNumber ;
            d->file_idx = foundNumber;
        }

        // only add images to d->files
        KMimeType::Ptr type = KMimeType::findByUrl(s);
        bool isImage        = type->name().contains("image", Qt::CaseInsensitive);

        if ( isImage )
        {
            d->files.append(s);
            foundNumber++;  //counter for searching the start image in case one image is selected
            kDebug() << s << " type=" << type->name() ;
        }
    }

    kDebug() << d->files.count() << "images loaded" ;

    // initialize cache
    for(int i = 0 ; i < CACHESIZE ; ++i)
    {
        d->cache[i].file_index = EMPTY;
        d->cache[i].texture    = new Texture;
    }

    if ( d->files.isEmpty() )
        return;

    showFullScreen();

    // let the cursor dissapear after 2sec of inactivity
    connect(&d->timerMouseMove, SIGNAL(timeout()),
            this, SLOT(slotTimeoutMouseMove()));

    d->timerMouseMove.start(2000);
    setMouseTracking(true);

    // other initialisations
    d->wheelAction = ViewerWidget::Private::changeImage;
}

ViewerWidget::~ViewerWidget()
{
    glDeleteTextures(1, d->tex);

    for(int i = 0 ; i < CACHESIZE ; ++i)
    {
        d->cache[i].file_index = EMPTY;
        delete d->cache[i].texture;
    }

    delete d;
}

/**
    \todo blending
 */
void ViewerWidget::initializeGL()
{
    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    // Clear The Background Color
    glClearColor(0.0, 0.0, 0.0, 1.0f);
    // Turn Blending On
    glEnable(GL_BLEND);
    // Blending Function For Translucency Based On Source Alpha Value
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Enable perspective vision
    glClearDepth(1.0f);
    // Generate d->texture
    glGenTextures(1, d->tex);
    //kDebug() << "width=" << width();
}

bool ViewerWidget::listOfFilesIsEmpty() const
{
    return d->files.isEmpty();
}

void ViewerWidget::paintGL()
{
    //this test has to be performed here since QWidget::width() is only updated now
    //kDebug() << "enter paintGL: isReallyFullscreen=" << isReallyFullScreen();
    //prepare 1st image
    if (d->firstImage && isReallyFullScreen())
    {
        //kDebug() << "first image";
        d->texture = loadImage(d->file_idx);
        d->texture->reset();
        downloadTexture(d->texture);

        //kDebug() << "width=" << width();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);
        drawImage(d->texture);

        //trigger a redraw NOW. the user wants to see a picture as soon as possible
        // only load the second image after the first is displayed
        glFlush();
        swapBuffers(); //TODO: this is probably not the right way to force a redraw

        //preload the 2nd image
        if (d->firstImage)
        {
            if (d->file_idx < (unsigned int)(d->files.count()-1))
            {
                loadImage(d->file_idx+1);
            }

            d->firstImage=false;
        }
    }

    if (!d->firstImage)
    {
        //kDebug() << "width=" << width();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);
        drawImage(d->texture);
    }

    //kDebug() << "exit paintGL";
}

void ViewerWidget::resizeGL(int w, int h)
{
    //kDebug() << "resizeGL,w=" << w;
    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (h>w)
    {
        d->ratio_view_x = 1.0;
        d->ratio_view_y = h/float(w);
    }
    else
    {
        d->ratio_view_x = w/float(h);
        d->ratio_view_y = 1.0;
    }

    glFrustum( -d->ratio_view_x, d->ratio_view_x, -d->ratio_view_y, d->ratio_view_y,5, 5000.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if (d->texture == 0)
        return;

    if (d->firstImage)
    {
        d->texture->setViewport(w,h);
    }
}

/**
    render the image
 */
void ViewerWidget::drawImage(Texture* const tex)
{
    // cout << "enter drawImage: target=" << d->texture->texnr() << " dim=" << d->texture->height() << " " << d->texture->width();
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, tex->texnr());
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(tex->vertex_left(), tex->vertex_bottom(), 0);

    glTexCoord2f(tex->width(), 0);
    glVertex3f(d->texture->vertex_right(), tex->vertex_bottom(), 0);

    glTexCoord2f(tex->width(), tex->height());
    glVertex3f(tex->vertex_right(), tex->vertex_top(), 0);

    glTexCoord2f(0, tex->height());
    glVertex3f(tex->vertex_left(), tex->vertex_top(), 0);
    glEnd();
}

/**
    Handle all keyboard events. All events which are not handled trigger
    a help window.
 */
void ViewerWidget::keyPressEvent(QKeyEvent* k)
{
    QPoint middlepoint;

    switch (k->key())
    {
        // next image
        case Qt::Key_N:
        case Qt::Key_Right:
        case Qt::Key_Down:
        case Qt::Key_PageDown:
        case Qt::Key_Space:
            nextImage();
            break;

        // previous image
        case Qt::Key_P:
        case Qt::Key_Left:
        case Qt::Key_Up:
        case Qt::Key_PageUp:
            prevImage();
            break;

        // rotate image
        case Qt::Key_R:
            d->texture->rotate();
            downloadTexture(d->texture);
            updateGL();
            break;

        // terminate image viewer
        case Qt::Key_Escape:
            // clean up: where does this have to be done?
            close();
            break;

        // full screen
        case Qt::Key_F:
            // according to QT documentation, showFullScreen() has some
            // serious issues on window managers that do not follow modern
            // post-ICCCM specifications
            if (isFullScreen())
            {
                d->texture->reset();
                showNormal();
            }
            else
            {
                d->texture->reset();
                showFullScreen();
            }
            break;

        // reset size and redraw
        case Qt::Key_Z:
            d->texture->reset();
            updateGL();
            break;

        // toggle permanent between "show next image" and "zoom" on mousewheel change
        case Qt::Key_C:
            if (d->wheelAction==ViewerWidget::Private::zoomImage)
                d->wheelAction=ViewerWidget::Private::changeImage;
            else
                d->wheelAction=ViewerWidget::Private::zoomImage;
            break;

        // zoom	in
        case Qt::Key_Plus:
            middlepoint = QPoint(width()/2,height()/2);
            if (d->texture->setSize( d->zoomsize ))
                downloadTexture(d->texture); //load full resolution image

            zoom(-1, middlepoint, d->zoomfactor_keyboard);
            break;

        // zoom out
        case Qt::Key_Minus:
            middlepoint = QPoint(width()/2,height()/2);
            if (d->texture->setSize( d->zoomsize ))
                downloadTexture(d->texture); //load full resolution image

            zoom(1, middlepoint, d->zoomfactor_keyboard);
            break;

        // zoom to original size
        case Qt::Key_O:
            d->texture->zoomToOriginal();
            updateGL();
            break;

        // toggle temorarily between "show next image" and "zoom" on mousewheel change
        case Qt::Key_Control:
            if (d->wheelAction == ViewerWidget::Private::zoomImage)
            {
                //scrollwheel changes to the next image
                d->wheelAction = ViewerWidget::Private::changeImage;
            }
            else
            {
                //scrollwheel does zoom
                d->wheelAction = ViewerWidget::Private::zoomImage;
                setCursor(d->zoomCursor);
                d->timerMouseMove.stop();
            }
            break;

        //do noting, don't trigger the help dialog
        case Qt::Key_Shift:
            break;

        //key is not bound to any action, therefore show help dialog to enlighten the user
        default:
            HelpDialog help;
            help.exec();
            break;
    }
}

void ViewerWidget::keyReleaseEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_Plus:
        case Qt::Key_Minus:
            if (!e->isAutoRepeat())
            {
                unsetCursor();
                if (d->texture->setSize(QSize(0, 0)))
                {
                    downloadTexture(d->texture); //load full resolution image
                }
                updateGL();
            }
            else
            {
                e->ignore();
            }
            break;

        case Qt::Key_Control:
            if (d->wheelAction == ViewerWidget::Private::zoomImage)
                d->wheelAction = ViewerWidget::Private::changeImage;
            else
                d->wheelAction = ViewerWidget::Private::zoomImage;
                unsetCursor();
                d->timerMouseMove.start(2000);
            break;

        default:
            e->ignore();
            break;
    }
}

/**
    download texture to video memory
 */
void ViewerWidget::downloadTexture(Texture* const tex)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, tex->texnr());
    // glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);
    // glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);

    // uncomment the following line to enable flat shading of texels -> debugging
    // glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, tex->width(), tex->height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, tex->data());
}

/**
    load d->files[file_index] into a texture object if it is not already cached
    \param file_index index to QStringList d->files
 */
Texture* ViewerWidget::loadImage(int file_index) const
{
    int imod = file_index%CACHESIZE; //index for cache

    if (d->cache[imod].file_index==file_index)
    {
        //image is already cached
        kDebug() << "image " << file_index << " is already in cache@" << imod ;
        return d->cache[imod].texture;
    }
    else
    {
        // image is net yet loaded
        QString f = d->files[file_index];
        kDebug() << "loading image " << f << "(idx=" << file_index << ") to cache@" << imod ;
        d->cache[imod].file_index=file_index;

        //when loadImage is called the first time, the frame is not yet fullscreen
        QSize size;

        if (d->firstImage)
        {
            //determine screensize since its not yet known by the widget
            QDesktopWidget dw;
            //QRect r = dw.screenGeometry(this);
            size    = dw.size();
            //kDebug() << "first image:size=" << size.width();
        }
        else
        {
            size = QSize(width(),height());
            //kDebug() << "next image:size=" << size.width();
        }

        // handle non-loadable images
        if (!d->cache[imod].texture->load(f, size, d->tex[0]))
        {
            d->cache[imod].texture->load(d->nullImage, size, d->tex[0]);
        }

        d->cache[imod].texture->setViewport(size.width(), size.height());
        return d->cache[imod].texture;
    }
}

void ViewerWidget::wheelEvent(QWheelEvent* e)
{
    switch(d->wheelAction)
    {
        // mousewheel triggers zoom
        case ViewerWidget::Private::zoomImage:
            setCursor(d->zoomCursor);
            zoom(e->delta(), e->pos(), d->zoomfactor_scrollwheel);
            break;

        // mousewheel triggers image change
        case ViewerWidget::Private::changeImage:
            if (e->delta() < 0)
                nextImage();
            else
                prevImage();
            break;
    }
}

void ViewerWidget::mousePressEvent(QMouseEvent* e)
{
    // begin zoom
    // scale down d->texture  for fast zooming
    // d->texture	 will be set to original size on mouse up
    if (d->texture->setSize( d->zoomsize ))
    {
        //load downsampled image
        downloadTexture(d->texture);
    }

    d->timerMouseMove.stop(); //user is something up to, therefore keep the cursor

    if ( e->button() == Qt::LeftButton )
    {
        setCursor(d->moveCursor);
    }

    if ( e->button() == Qt::RightButton )
    {
        setCursor(d->zoomCursor);
    }

    d->startdrag    = e->pos();
    d->previous_pos = e->pos();
}

void ViewerWidget::mouseMoveEvent(QMouseEvent* e)
{
    if ( e->buttons() == Qt::LeftButton )
    {
        //panning
        setCursor(d->moveCursor);
        QPoint diff = e->pos()-d->startdrag;
        d->texture->move(diff);
        updateGL();
        d->startdrag = e->pos();
    }
    else if ( e->buttons() == Qt::RightButton )
    {
        int mdelta = 0;

        //zooming
        //
        //if mouse pointer reached upper or lower boder, special treatment in order
        //to keep zooming enabled in that special case
        if ( d->previous_pos.y() == e->y() )
        {
            if ( e->y() == 0 )
            {
                // mouse pointer is at upper edge, therefore assume zoom in
                mdelta = 1;
            }
            else
            {
                // mouse pointer is as lower edge, therefore assume zoom out
                mdelta = -1;
            }
        }
        else
        {
            // mouse pointer is in the middle of the screen, normal operation
            mdelta = d->previous_pos.y()-e->y();
        }

        setCursor(d->zoomCursor);
        zoom(mdelta, d->startdrag, d->zoomfactor_mousemove);
        d->previous_pos = e->pos();
    }
    else
    {
        //no key is pressed while moving mouse
        //don't do anything if ctrl is pressed
        if (d->timerMouseMove.isActive())
        {
            //ctrl is not pressed, no zooming, therefore restore and hide cursor in 2 sec
            unsetCursor();
            d->timerMouseMove.start(2000);
        }
    }

    return;
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent*)
{
    d->timerMouseMove.start(2000);
    unsetCursor();

    if (d->texture->setSize(QSize(0,0)))
    {
        //load full resolution image
        downloadTexture(d->texture);
    }

    updateGL();
}

/**
    a double click resets the view (zoom and move)
 */
void ViewerWidget::mouseDoubleClickEvent(QMouseEvent*)
{
    d->texture->reset();
    updateGL();
}

void ViewerWidget::prevImage()
{
#ifdef PERFORMANCE_ANALYSIS
    Timer timer;
#endif

    if (d->file_idx>0)
        d->file_idx--;
    else
        return;

#ifdef PERFORMANCE_ANALYSIS
    timer.start();
#endif

    d->texture = loadImage(d->file_idx);
    d->texture->reset();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("loadImage");
#endif

    downloadTexture(d->texture);

#ifdef PERFORMANCE_ANALYSIS
    timer.at("downloadTexture");
#endif

    updateGL();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("updateGL");
#endif

    //image preloading
    if (d->file_idx>0)
        loadImage(d->file_idx-1);
}

void ViewerWidget::nextImage()
{
#ifdef PERFORMANCE_ANALYSIS
    Timer timer;
#endif

    if (d->file_idx < (unsigned int)(d->files.count()-1))
        d->file_idx++;
    else
        return;

#ifdef PERFORMANCE_ANALYSIS
    timer.start();
#endif

    d->texture = loadImage(d->file_idx);
    d->texture->reset();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("loadImage");
#endif

    downloadTexture(d->texture);

#ifdef PERFORMANCE_ANALYSIS
    timer.at("downloadTexture");
#endif

    updateGL();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("updateGL");
#endif

    //image preloading
    if (d->file_idx < ((unsigned int)d->files.count()-1))
    {
        loadImage(d->file_idx+1);

#ifdef PERFORMANCE_ANALYSIS
        timer.at("preloading");
#endif
    }
}

/**
    \param mdelta delta of mouse movement:
                                mdelta>0: zoom in
                                mdelta<0: zoom out
                                mdelta=0: do nothing
    \param pos position of mouse
    \param factor zoom factor:scrollwheel needs a higher factor that right click mouse move. factor=1 -> no zoom
 */
void ViewerWidget::zoom(int mdelta, const QPoint& pos, float factor)
{
    if (mdelta==0)
    {
        //do nothing
        return;
    }

    if (mdelta > 0)
    {
        //multiplicator for zooming in
        d->delta = factor;
    }

    if (mdelta < 0)
    {
        //multiplicator for zooming out
        d->delta = 2.0-factor;
    }

    d->texture->zoom(d->delta,pos);
    updateGL();
}

/**
    being called if user didn't move the mouse for longer than 2 sec
 */
void ViewerWidget::slotTimeoutMouseMove()
{
    setCursor(Qt::BlankCursor);
}

/**
    check if OpenGL engine is ready. This function is called from outside the widget.
    If OpenGL doen't work correctly, the widget can be destroyed
    \return OGLstate::oglNoContext No OpenGl context could be retrieved
    \return OGLstate::oglNoRectangularTexture GLGL_ARB_texture_rectangle is not supported
    \return OGLstate::oglOK all is fine
 */
OGLstate ViewerWidget::getOGLstate() const
{
    //no OpenGL context is found. Are the drivers ok?
    if ( !isValid() )
    {
        return oglNoContext;
    }

    //GL_ARB_texture_rectangle is not supported
    QString s = QString ( ( char* ) glGetString ( GL_EXTENSIONS ) );

    if ( !s.contains ( "GL_ARB_texture_rectangle",Qt::CaseInsensitive ) )
    {
        return oglNoRectangularTexture;
    }

    //everything is ok!
    return oglOK;
}

/**
    QGLWidget::isFullscreen() returns true if the internal state is already true
    but the actually displayed size is still windowed. isReallyFullscreen() returns the
    value of the visible size.
    \return true if (screenwidth == widgedwidth)
 */
bool ViewerWidget::isReallyFullScreen() const
{
    return (width() == d->screen_width);
}

} // namespace KIPIViewerPlugin
