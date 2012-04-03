/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Local includes

#include "timer.h"
#include "texture.h"
#include "ui_helpdialog.h"

#ifndef GL_TEXTURE_RECTANGLE_ARB
#define GL_TEXTURE_RECTANGLE_ARB   0x84F5
#endif

#ifndef GL_TEXTURE_RECTANGLE_NV
#define GL_TEXTURE_RECTANGLE_NV    0x84F5
#endif

namespace KIPIViewerPlugin
{

ViewerWidget::ViewerWidget(Interface* const iface)
{
    m_kipiInterface           = iface;
    ImageCollection selection = m_kipiInterface->currentSelection();
    ImageCollection album     = m_kipiInterface->currentAlbum();

    KUrl::List myfiles; //pics which are displayed in imageviewer
    QString selectedImage; //selected pic in hostapp

    int foundNumber = 0;
    m_texture       = 0;
    m_file_idx      = 0; //index of picture to be displayed

    //determine screen size for isReallyFullScreen
    QDesktopWidget dw;
    m_screen_width = dw.screenGeometry(this).width();

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

    // populate QStringList::m_files
    for(KUrl::List::Iterator it = myfiles.begin(); it != myfiles.end(); ++it)
    {
        // find selected image in album in order to determine the first displayed image
        // in case one image was selected and the entire album was loaded
        it->cleanPath();
        QString s = (*it).path();

        if ( s == selectedImage )
        {
            kDebug() << "selected img  " << selectedImage << " has idx=" << foundNumber ;
            m_file_idx = foundNumber;
        }

        // only add images to m_files
        KMimeType::Ptr type = KMimeType::findByUrl(s);
        bool isImage        = type->name().contains("image", Qt::CaseInsensitive);

        if ( isImage )
        {
            m_files.append(s);
            foundNumber++;  //counter for searching the start image in case one image is selected
            kDebug() << s << " type=" << type->name() ;
        }
    }

    m_firstImage = true;
    kDebug() << m_files.count() << "images loaded" ;

    // initialize cache
    for(int i = 0 ; i < CACHESIZE ; ++i)
    {
        m_cache[i].file_index = EMPTY;
        m_cache[i].texture    = new Texture;
    }

    if ( m_files.isEmpty() )
        return;

    // define zoomfactors for one zoom step
    m_zoomfactor_scrollwheel = 1.1F;
    m_zoomfactor_mousemove   = 1.03F;
    m_zoomfactor_keyboard    = 1.05F;

    // load cursors for zooming and panning
    QString file;
    file         = KStandardDirs::locate( "data", "kipiplugin_imageviewer/pics/zoom.png" );
    m_zoomCursor = QCursor(QPixmap(file));
    file         = KStandardDirs::locate( "data", "kipiplugin_imageviewer/pics/hand.png" );
    m_zoomCursor = QCursor(QPixmap(file));

    // get path of nullImage in case QImage can't load the image
    m_nullImage  = KStandardDirs::locate( "data", "kipiplugin_imageviewer/pics/nullImage.png" );

    showFullScreen();

    // let the cursor dissapear after 2sec of inactivity
    connect(&m_timerMouseMove, SIGNAL(timeout()),
            this, SLOT(timeoutMouseMove()));

    m_timerMouseMove.start(2000);
    setMouseTracking(true);

    // while zooming is performed, the image is downsampled to m_zoomsize. This seems to
    // be the optimal way for a PentiumM 1.4G, Nvidia FX5200. For a faster setup, this might
    // not be necessary anymore
    m_zoomsize    = QSize(1024,768);

    // other initialisations
    m_wheelAction = changeImage;
}

ViewerWidget::~ViewerWidget()
{
    glDeleteTextures(1, m_tex);
    for(int i = 0 ; i < CACHESIZE ; ++i)
    {
        m_cache[i].file_index = EMPTY;
        delete m_cache[i].texture;
    }
}

/*!
    \fn ViewerWidget::initializeGL()
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
    // Generate m_texture
    glGenTextures(1, m_tex);
    //kDebug() << "width=" << width();
}

bool ViewerWidget::listOfFilesIsEmpty() const
{
    return m_files.isEmpty();
}

/*!
    \fn ViewerWidget::paintGL()
 */
void ViewerWidget::paintGL()
{
    //this test has to be performed here since QWidget::width() is only updated now
    //kDebug() << "enter paintGL: isReallyFullscreen=" << isReallyFullScreen();
    //prepare 1st image
    if (m_firstImage && isReallyFullScreen())
    {
        //kDebug() << "first image";
        m_texture = loadImage(m_file_idx);
        m_texture->reset();
        downloadTex(m_texture);

        //kDebug() << "width=" << width();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);
        drawImage(m_texture);

        //trigger a redraw NOW. the user wants to see a picture as soon as possible
        // only load the second image after the first is displayed
        glFlush();
        swapBuffers(); //TODO: this is probably not the right way to force a redraw

        //preload the 2nd image
        if (m_firstImage)
        {
            if (m_file_idx < (unsigned int)(m_files.count()-1))
            {
                loadImage(m_file_idx+1);
            }
            m_firstImage=false;
        }
    }

    if (!m_firstImage)
    {
        //kDebug() << "width=" << width();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);
        drawImage(m_texture);
    }

    //kDebug() << "exit paintGL";
}

/*!
    \fn ViewerWidget::resizeGL(int w, int h)
 */
void ViewerWidget::resizeGL(int w, int h)
{
    //kDebug() << "resizeGL,w=" << w;
    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (h>w)
    {
        m_ratio_view_x = 1.0;
        m_ratio_view_y = h/float(w);
    }
    else
    {
        m_ratio_view_x = w/float(h);
        m_ratio_view_y = 1.0;
    }

    glFrustum( -m_ratio_view_x, m_ratio_view_x, -m_ratio_view_y, m_ratio_view_y,5, 5000.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if (m_texture == 0)
        return;

    if (m_firstImage)
    {
        m_texture->setViewport(w,h);
    }
}

/*!
    \fn ViewerWidget::drawImage(Texture* m_texture)
    \brief render the image
 */
void ViewerWidget::drawImage(Texture* const m_texture)
{
// 	cout << "enter drawImage: target=" << m_texture->texnr() << " dim=" << m_texture->height() << " " << m_texture->width();
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texture->texnr());
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(m_texture->vertex_left(), m_texture->vertex_bottom(), 0);

    glTexCoord2f(m_texture->width(), 0);
    glVertex3f(m_texture->vertex_right(), m_texture->vertex_bottom(), 0);

    glTexCoord2f(m_texture->width(), m_texture->height());
    glVertex3f(m_texture->vertex_right(), m_texture->vertex_top(), 0);

    glTexCoord2f(0, m_texture->height());
    glVertex3f(m_texture->vertex_left(), m_texture->vertex_top(), 0);
    glEnd();
}

/*!
    \fn ViewerWidget::keyPressEvent(QKeyEvent *k)
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
            m_texture->rotate();
            downloadTex(m_texture);
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
                m_texture->reset();
                showNormal();
            }
            else
            {
                m_texture->reset();
                showFullScreen();
            }
            break;

        // reset size and redraw
        case Qt::Key_Z:
            m_texture->reset();
            updateGL();
            break;

        // toggle permanent between "show next image" and "zoom" on mousewheel change
        case Qt::Key_C:
            if (m_wheelAction==zoomImage)
                m_wheelAction=changeImage;
            else
                m_wheelAction=zoomImage;
            break;

        // zoom	in
        case Qt::Key_Plus:
            middlepoint =  QPoint(width()/2,height()/2);
            if (m_texture->setSize( m_zoomsize ))
                downloadTex(m_texture); //load full resolution image

            zoom(-1, middlepoint, m_zoomfactor_keyboard);
            break;

        // zoom out
        case Qt::Key_Minus:
            middlepoint =  QPoint(width()/2,height()/2);
            if (m_texture->setSize( m_zoomsize ))
                downloadTex(m_texture); //load full resolution image

            zoom(1, middlepoint, m_zoomfactor_keyboard);
            break;

        // zoom to original size
        case Qt::Key_O:
            m_texture->zoomToOriginal();
            updateGL();
            break;

        // toggle temorarily between "show next image" and "zoom" on mousewheel change
        case Qt::Key_Control:
            if (m_wheelAction == zoomImage)
            {
                //scrollwheel changes to the next image
                m_wheelAction = changeImage;
            }
            else
            {
                //scrollwheel does zoom
                m_wheelAction = zoomImage;
                setCursor (m_zoomCursor);
                m_timerMouseMove.stop();
            }
            break;

        //do noting, don't trigger the help dialog
        case Qt::Key_Shift:
            break;

        //key is not bound to any action, therefore show help dialog to enlighten the user
        default:
            QPointer<QDialog> d = new QDialog(this);
            Ui::HelpDialog hd;
            hd.setupUi(d);
            d->exec();
            delete d;
            break;
    }
}

/*!
    \fn ViewerWidget::keyReleaseEvent ( QKeyEvent * e )
 */
void ViewerWidget::keyReleaseEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_Plus:
        case Qt::Key_Minus:
            if (!e->isAutoRepeat())
            {
                unsetCursor();
                if (m_texture->setSize(QSize(0, 0)))
                {
                    downloadTex(m_texture); //load full resolution image
                }
                updateGL();
            }
            else
            {
                e->ignore();
            }
            break;

        case Qt::Key_Control:
            if (m_wheelAction == zoomImage)
                m_wheelAction = changeImage;
            else
                m_wheelAction = zoomImage;
                unsetCursor();
                m_timerMouseMove.start(2000);
            break;

        default:
            e->ignore();
            break;
    }
}

/*!
    \fn ViewerWidget::downloadTex(Texture* tex)
    download m_texture to video memory
 */
void ViewerWidget::downloadTex(Texture* const tex)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, tex->texnr());
    // glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);
    // glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);

//	uncomment the following line to enable flat shading of texels -> debugging
//	glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, tex->width(), tex->height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, tex->data());
}

/*!
    \fn ViewerWidget::loadImage(int file_index)
    \param file_index index to QStringList m_files
    load m_files[file_index] into a texture object if it is not already cached
 */
Texture* ViewerWidget::loadImage(int file_index)
{
    int imod = file_index%CACHESIZE; //index for cache
    if (m_cache[imod].file_index==file_index)
    {
        //image is already cached
        kDebug() << "image " << file_index << " is already in cache@" << imod ;
        return m_cache[imod].texture;

    }
    else
    {
        // image is net yet loaded
        QString f = m_files[file_index];
        kDebug() << "loading image " << f << "(idx=" << file_index << ") to cache@" << imod ;
        m_cache[imod].file_index=file_index;

        //when loadImage is called the first time, the frame is not yet fullscreen
        QSize size;
        if (m_firstImage)
        {
            //determine screensize since its not yet known by the widget
            QDesktopWidget dw;
            QRect r = dw.screenGeometry(this);
            size    = dw.size();
            //kDebug() << "first image:size=" << size.width();
        }
        else
        {
            size = QSize(width(),height());
            //kDebug() << "next image:size=" << size.width();
        }

        // handle non-loadable images
        if (!m_cache[imod].texture->load(f, size, m_tex[0]))
        {
            m_cache[imod].texture->load(m_nullImage, size, m_tex[0]);
        }

        m_cache[imod].texture->setViewport(size.width(), size.height());
        return m_cache[imod].texture;
    }
}

/*!
    \fn ViewerWidget::wheelEvent(QWheelEvent* e)
 */
void ViewerWidget::wheelEvent(QWheelEvent* e)
{
    switch(m_wheelAction)
    {
        // mousewheel triggers zoom
        case zoomImage:
            setCursor(m_zoomCursor);
            zoom(e->delta(), e->pos(), m_zoomfactor_scrollwheel);
            break;

        // mousewheel triggers image change
        case changeImage:
            if (e->delta() < 0)
                nextImage();
            else
                prevImage();
            break;
    }
}

/*!
    \fn ViewerWidget::mousePressEvent(QMouseEvent* e)
 */
void ViewerWidget::mousePressEvent(QMouseEvent* e)
{
    // begin zoom
    // scale down m_texture  for fast zooming
    // m_texture	 will be set to original size on mouse up
    if (m_texture->setSize( m_zoomsize ))
    {
        //load downsampled image
        downloadTex(m_texture);
    }

    m_timerMouseMove.stop(); //user is something up to, therefore keep the cursor
    if ( e->button() == Qt::LeftButton )
    {
        setCursor (m_zoomCursor); //defined in constructor
    }

    if ( e->button() == Qt::RightButton )
    {
        setCursor (m_zoomCursor);
    }

    m_startdrag    = e->pos();
    m_previous_pos = e->pos();
}

/*!
    \fn ViewerWidget::mouseMoveEvent(QMouseEvent* e)
 */
void ViewerWidget::mouseMoveEvent(QMouseEvent* e)
{
    int mdelta;
    if ( e->buttons() == Qt::LeftButton )
    {
        //panning
        QPoint diff = e->pos()-m_startdrag;
        m_texture->move(diff);
        updateGL();
        m_startdrag = e->pos();
    }
    else if ( e->buttons() == Qt::RightButton )
    {
        //zooming
        //
        //if mouse pointer reached upper or lower boder, special treatment in order
        //to keep zooming enabled in that special case
        if ( m_previous_pos.y() == e->y() )
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
            mdelta = m_previous_pos.y()-e->y();
        }

        zoom(mdelta, m_startdrag, m_zoomfactor_mousemove );
        m_previous_pos = e->pos();
    }
    else
    {
        //no key is pressed while moving mouse
        //don't do anything if ctrl is pressed
        if (m_timerMouseMove.isActive())
        {
            //ctrl is not pressed, no zooming, therefore restore and hide cursor in 2 sec
            unsetCursor();
            m_timerMouseMove.start(2000);
        }
    }
    return;
}

/*!
    \fn ViewerWidget::prevImage()
 */
void ViewerWidget::prevImage()
{
#ifdef PERFORMANCE_ANALYSIS
    Timer timer;
#endif

    if (m_file_idx>0)
        m_file_idx--;
    else
        return;

#ifdef PERFORMANCE_ANALYSIS
    timer.start();
#endif

    m_texture = loadImage(m_file_idx);
    m_texture->reset();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("loadImage");
#endif

    downloadTex(m_texture);

#ifdef PERFORMANCE_ANALYSIS
    timer.at("downloadTex");
#endif

    updateGL();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("updateGL");
#endif

    //image preloading
    if (m_file_idx>0)
        loadImage(m_file_idx-1);
}

/*!
    \fn ViewerWidget::nextImage()
 */
void ViewerWidget::nextImage()
{
#ifdef PERFORMANCE_ANALYSIS
    Timer timer;
#endif

    if (m_file_idx < (unsigned int)(m_files.count()-1))
        m_file_idx++;
    else
        return;

#ifdef PERFORMANCE_ANALYSIS
    timer.start();
#endif

    m_texture = loadImage(m_file_idx);
    m_texture->reset();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("loadImage");
#endif

    downloadTex(m_texture);

#ifdef PERFORMANCE_ANALYSIS
    timer.at("downloadTex");
#endif

    updateGL();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("updateGL");
#endif

    //image preloading
    if (m_file_idx < ((unsigned int)m_files.count()-1))
    {
        loadImage(m_file_idx+1);

#ifdef PERFORMANCE_ANALYSIS
        timer.at("preloading");
#endif
    }
}

/*!
    \fn ViewerWidget::zoom(int mdelta, QPos pos)
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
        m_delta = factor;
    }

    if (mdelta < 0)
    {
        //multiplicator for zooming out
        m_delta = 2.0-factor;
    }

    m_texture->zoom(m_delta,pos);
    updateGL();
}

/*!
    \fn ViewerWidget::mouseDoubleClickEvent(QMouseEvent* e)
    a double click resets the view (zoom and move)
 */
void ViewerWidget::mouseDoubleClickEvent(QMouseEvent*)
{
    m_texture->reset();
    updateGL();
}

/*!
    \fn ViewerWidget::mouseReleaseEvent(QMouseEvent* e)
 */
void ViewerWidget::mouseReleaseEvent(QMouseEvent*)
{
    m_timerMouseMove.start(2000);
    unsetCursor();
    if (m_texture->setSize(QSize(0,0)))
    {
        //load full resolution image
        downloadTex(m_texture);
    }
    updateGL();
}

/*!
    \fn ViewerWidget::timeoutMouseMove()
    being called if user didn't move the mouse for longer than 2 sec
 */
void ViewerWidget::timeoutMouseMove()
{
    setCursor (QCursor (Qt::BlankCursor));
}

/*!
    \fn ViewerWidget::getOGLstate() const
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

/*!
    \fn ViewerWidget::isReallyFullScreen()
    QGLWidget::isFullscreen() returns true if the internal state is already true
    but the actually displayed size is still windowed. isReallyFullscreen() returns the
    value of the visible size.
    \return true if screenwidth==widgedwidth
 */
bool ViewerWidget::isReallyFullScreen() const
{
    return (width() == m_screen_width);
}

} // namespace KIPIViewerPlugin
