/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using
 *               an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
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

#undef PERFORMANCE_ANALYSIS

#include "viewerwidget.h"
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

//#define PERFORMANCE_ANALYSIS
#ifdef PERFORMANCE_ANALYSIS
    #include "timer.h"
#endif

#include "texture.h"
#include "ui_helpdialog.h"

#ifndef GL_TEXTURE_RECTANGLE_ARB
#define GL_TEXTURE_RECTANGLE_ARB   0x84F5
#endif

#ifndef GL_TEXTURE_RECTANGLE_NV
#define GL_TEXTURE_RECTANGLE_NV    0x84F5
#endif

// using namespace std;
using namespace KIPIviewer;

ViewerWidget::ViewerWidget(KIPI::Interface* i) {

        kipiInterface = i;

        KIPI::ImageCollection selection = kipiInterface->currentSelection();
        KIPI::ImageCollection album = kipiInterface->currentAlbum();

        KUrl::List myfiles; //pics which are displayed in imageviewer
        QString selectedImage; //selected pic in hostapp

        int foundNumber=0;
        texture=0;
        file_idx=0; //index of picture to be displayed

        //determine screen size for isReallyFullScreen
        QDesktopWidget dw;
        screen_width = dw.screenGeometry(this).width();

        if ( selection.images().count()==0 ) {
            kDebug() << "no image selected, load entire album" ;
            myfiles = album.images();
        }
        else if ( selection.images().count()==1 ) {
            kDebug() << "one image selected, load entire album and start with selected image" ;
            selectedImage = selection.images().first().path();
            myfiles = album.images();
        }
        else if ( selection.images().count()>1 ) {
            kDebug() << "load " << selection.images().count() << " selected images" ;
            myfiles = selection.images();
        }

        // populate QStringList::files
        for(KUrl::List::Iterator it = myfiles.begin(); it != myfiles.end();it++) {

            // find selected image in album in order to determine the first displayed image
            // in case one image was selected and the entire album was loaded
            it->cleanPath();
            QString s = (*it).path();
            if ( s==selectedImage ) {
                kDebug() << "selected img  " << selectedImage << " has idx=" << foundNumber ;
                file_idx=foundNumber;
            }

            // only add images to files
            KMimeType::Ptr type = KMimeType::findByUrl(s);
            bool isImage=type->name().contains("image",Qt::CaseInsensitive);

            if ( isImage ) {
                files.append(s);
                foundNumber++;  //counter for searching the start image in case one image is selected
                kDebug() << s << " type=" << type->name() ;
            }
        }

        firstImage=true;
        kDebug() << files.count() << "images loaded" ;
        // initialize cache
        for(int i=0;i<CACHESIZE;i++) {
            cache[i].file_index=EMPTY;
            cache[i].texture=new Texture(kipiInterface);
        }
                if ( files.isEmpty() )
                    return;

        // define zoomfactors for one zoom step
        zoomfactor_scrollwheel = 1.1F;
        zoomfactor_mousemove   = 1.03F;
        zoomfactor_keyboard    = 1.05F;

        // load cursors for zooming and panning
        QString file;
        file = KStandardDirs::locate( "data", "kipiplugin_imageviewer/pics/zoom.png" );
        zoomCursor=QCursor(QPixmap(file));
        file = KStandardDirs::locate( "data", "kipiplugin_imageviewer/pics/hand.png" );
        moveCursor=QCursor(QPixmap(file));

        // get path of nullImage in case QImage can't load the image
        nullImage = KStandardDirs::locate( "data", "kipiplugin_imageviewer/pics/nullImage.png" );

        showFullScreen();

        // let the cursor dissapear after 2sec of inactivity
        connect( &timerMouseMove, SIGNAL(timeout()),this, SLOT( timeoutMouseMove()) );
        timerMouseMove.start(2000);
        setMouseTracking(true);

        // while zooming is performed, the image is downsampled to zoomsize. This seems to
        // be the optimal way for a PentiumM 1.4G, Nvidia FX5200. For a faster setup, this might
        // not be necessary anymore
        zoomsize=QSize(1024,768);

        // other initialisations
        wheelAction = changeImage;
}


/*!
    \fn ViewerWidget::initializeGL()
    \todo blending
 */
void ViewerWidget::initializeGL() {
    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    // Clear The Background Color
    glClearColor(0.0, 0.0, 0.0, 1.0f);
    // Turn Blending On
    glEnable(GL_BLEND);
    // Blending Function For Translucency Based On Source Alpha Value
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // Enable perspective vision
    glClearDepth(1.0f);
    // Generate texture
    glGenTextures(1, tex);
    //kDebug() << "width=" << width();
}


bool ViewerWidget::listOfFilesIsEmpty() const
{
    return files.isEmpty();
}


/*!
    \fn ViewerWidget::paintGL()
 */
void ViewerWidget::paintGL() {

    //this test has to be performed here since QWidget::width() is only updated now
    //kDebug() << "enter paintGL: isReallyFullscreen=" << isReallyFullScreen();
    //prepare 1st image
    if (firstImage && isReallyFullScreen()) {
        //kDebug() << "first image";
        texture=loadImage(file_idx);
        texture->reset();
        downloadTex(texture);

        //kDebug() << "width=" << width();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.0f,0.0f,-5.0f);
        drawImage(texture);

        //trigger a redraw NOW. the user wants to see a picture as soon as possible
        // only load the second image after the first is displayed
        glFlush();
        swapBuffers(); //TODO: this is probably not the right way to force a redraw

        //preload the 2nd image
        if (firstImage) {
            if (file_idx < (unsigned int)(files.count()-1)) {
                loadImage(file_idx+1);
            }
            firstImage=false;
        }
    }

    if (!firstImage) {
        //kDebug() << "width=" << width();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.0f,0.0f,-5.0f);
        drawImage(texture);
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

    if (h>w) {
        ratio_view_x=1.0;
        ratio_view_y=h/float(w);
    }
    else {
        ratio_view_x=w/float(h);
        ratio_view_y=1.0;
    }

    glFrustum( -ratio_view_x, ratio_view_x, -ratio_view_y, ratio_view_y,5, 5000.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if (texture == 0)
        return;

    if (firstImage) {
        texture->setViewport(w,h);
    }
}





/*!
    \fn ViewerWidget::drawImage(Texture * texture)
    \brief render the image
 */
void ViewerWidget::drawImage(Texture * texture)
{
// 	cout << "enter drawImage: target=" << texture->texnr() << " dim=" << texture->height() << " " << texture->width();
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, texture->texnr());
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(texture->vertex_left(), texture->vertex_bottom(), 0);

        glTexCoord2f(texture->width(), 0);
        glVertex3f(texture->vertex_right(), texture->vertex_bottom(), 0);

        glTexCoord2f(texture->width(), texture->height());
        glVertex3f(texture->vertex_right(), texture->vertex_top(), 0);

        glTexCoord2f(0, texture->height());
        glVertex3f(texture->vertex_left(), texture->vertex_top(), 0);
    glEnd();
}


/*!
    \fn ViewerWidget::keyPressEvent(QKeyEvent *k)
    Handle all keyboard events. All events which are not handled trigger
    a help window.
 */
void ViewerWidget::keyPressEvent(QKeyEvent *k)
{
    QPoint middlepoint;

    switch (k->key()) {
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
            texture->rotate();
            downloadTex(texture);
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
            if (isFullScreen()) {
                texture->reset();
                showNormal();
            } else {
                texture->reset();
                showFullScreen();
            }
            break;

        // reset size and redraw
        case Qt::Key_Z:
            texture->reset();
            updateGL();
            break;

        // toggle permanent between "show next image" and "zoom" on mousewheel change
        case Qt::Key_C:
            if (wheelAction==zoomImage)
                wheelAction=changeImage;
            else
                wheelAction=zoomImage;
            break;

        // zoom	in
        case Qt::Key_Plus:
            middlepoint =  QPoint(width()/2,height()/2);
            if (texture->setSize( zoomsize )) {
                downloadTex(texture); //load full resolution image
            };
            zoom(-1, middlepoint, zoomfactor_keyboard);
            break;

        // zoom out
        case Qt::Key_Minus:
            middlepoint =  QPoint(width()/2,height()/2);
            if (texture->setSize( zoomsize ))
                downloadTex(texture); //load full resolution image
            zoom(1, middlepoint, zoomfactor_keyboard);
            break;

        // zoom to original size
        case Qt::Key_O:
            texture->zoomToOriginal();
            updateGL();
            break;

        // toggle temorarily between "show next image" and "zoom" on mousewheel change
        case Qt::Key_Control:
            if (wheelAction==zoomImage)
                //scrollwheel changes to the next image
                wheelAction=changeImage;
            else {
                //scrollwheel does zoom
                wheelAction=zoomImage;
                setCursor (zoomCursor);
                timerMouseMove.stop();
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
void ViewerWidget::keyReleaseEvent ( QKeyEvent * e )
{
    switch (e->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Minus:
            if (!e->isAutoRepeat()) {
                unsetCursor();
                if (texture->setSize(QSize(0,0))) {
                    downloadTex(texture); //load full resolution image
                }
                updateGL();
            } else
                e->ignore();
            break;

        case Qt::Key_Control:
            if (wheelAction==zoomImage)
                wheelAction=changeImage;
            else
                wheelAction=zoomImage;
                unsetCursor();
                timerMouseMove.start(2000);
            break;

        default:
            e->ignore();
            break;
    }

}


/*!
    \fn ViewerWidget::downloadTex(Texture * tex)
    download texture to video memory
 */
void ViewerWidget::downloadTex(Texture * tex)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, tex->texnr());
    // glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);
    // glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);

//	uncomment the following line to enable flat shading of texels -> debugging
//	glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, tex->width(), tex->height(), 0,GL_RGBA, GL_UNSIGNED_BYTE, tex->data());
}



/*!
    \fn ViewerWidget::loadImage(int file_index)
    \param file_index index to QStringList files
    load files[file_index] into a texture object if it is not already cached
 */
Texture * ViewerWidget::loadImage(int file_index)
{
    int imod=file_index%CACHESIZE; //index for cache
    if (cache[imod].file_index==file_index){
        //image is already cached
        kDebug() << "image " << file_index << " is already in cache@" << imod ;
        return cache[imod].texture;

    } else {
        // image is net yet loaded
        QString f = files[file_index];
        kDebug() << "loading image " << f << "(idx=" << file_index << ") to cache@" << imod ;
        cache[imod].file_index=file_index;

        //when loadImage is called the first time, the frame is not yet fullscreen
        QSize size;
        if (firstImage) {
            //determine screensize since its not yet known by the widget
                QDesktopWidget dw;
            QRect r = dw.screenGeometry(this);
            size=dw.size();
            //kDebug() << "first image:size=" << size.width();
        } else {
            size = QSize(width(),height());
            //kDebug() << "next image:size=" << size.width();
        }

        // handle non-loadable images
        if (!cache[imod].texture->load(f,size,tex[0])) {
            cache[imod].texture->load(nullImage,size,tex[0]);
        }

        cache[imod].texture->setViewport(size.width(),size.height());
        return cache[imod].texture;
    }
}


/*!
    \fn ViewerWidget::wheelEvent ( QWheelEvent * e )
 */
void ViewerWidget::wheelEvent ( QWheelEvent * e )
{
    switch(wheelAction) {
        // mousewheel triggers zoom
        case zoomImage:
            setCursor(zoomCursor);
            zoom(e->delta(), e->pos(), zoomfactor_scrollwheel);
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
    \fn ViewerWidget::mousePressEvent ( QMouseEvent * e )
 */
void ViewerWidget::mousePressEvent ( QMouseEvent * e )
{
    // begin zoom
    // scale down texture  for fast zooming
    // texture	 will be set to original size on mouse up
    if (texture->setSize( zoomsize )) { //load downsampled image
        downloadTex(texture);
    }

    timerMouseMove.stop(); //user is something up to, therefore keep the cursor
    if ( e->button() == Qt::LeftButton ) {
        setCursor (moveCursor); //defined in constructor
    }

    if ( e->button() == Qt::RightButton ) {
        setCursor (zoomCursor);
    }

    startdrag=e->pos();
    previous_pos=e->pos();
}


/*!
    \fn ViewerWidget::mouseMoveEvent ( QMouseEvent * e )
 */
void ViewerWidget::mouseMoveEvent ( QMouseEvent * e )
{
    int mdelta;
    if ( e->buttons() == Qt::LeftButton ) {
        //panning
        QPoint diff=e->pos()-startdrag;
        texture->move(diff);
        updateGL();
        startdrag=e->pos();
    } else if ( e->buttons() == Qt::RightButton ) {
        //zooming
        //
        //if mouse pointer reached upper or lower boder, special treatment in order
        //to keep zooming enabled in that special case
        if ( previous_pos.y() == e->y() ) {
            if ( e->y() == 0 ) {
                // mouse pointer is at upper edge, therefore assume zoom in
                mdelta = 1;
            } else {
                // mouse pointer is as lower edge, therefore assume zoom out
                mdelta = -1;
            }
        } else {
            // mouse pointer is in the middle of the screen, normal operation
            mdelta = previous_pos.y()-e->y();
        }

        zoom(mdelta, startdrag, zoomfactor_mousemove );
        previous_pos=e->pos();
    } else {
        //no key is pressed while moving mouse
        //don't do anything if ctrl is pressed
        if (timerMouseMove.isActive()) {
            //ctrl is not pressed, no zooming, therefore restore and hide cursor in 2 sec
            unsetCursor();
            timerMouseMove.start(2000);
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

    if (file_idx>0)
        file_idx--;
    else
        return;
#ifdef PERFORMANCE_ANALYSIS
    timer.start();
#endif
    texture = loadImage(file_idx);
    texture->reset();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("loadImage");
#endif

    downloadTex(texture);

#ifdef PERFORMANCE_ANALYSIS
    timer.at("downloadTex");
#endif

    updateGL();

#ifdef PERFORMANCE_ANALYSIS
    timer.at("updateGL");
#endif

    //image preloading
    if (file_idx>0)
        loadImage(file_idx-1);
}


/*!
    \fn ViewerWidget::nextImage()
 */
void ViewerWidget::nextImage()
{
#ifdef PERFORMANCE_ANALYSIS
    Timer timer;
#endif
    if (file_idx < (unsigned int)(files.count()-1))
        file_idx++;
    else
        return;
#ifdef PERFORMANCE_ANALYSIS
    timer.start();
#endif
    texture = loadImage(file_idx);
    texture->reset();
#ifdef PERFORMANCE_ANALYSIS
    timer.at("loadImage");
#endif
    downloadTex(texture);
#ifdef PERFORMANCE_ANALYSIS
    timer.at("downloadTex");
#endif
    updateGL();
#ifdef PERFORMANCE_ANALYSIS
    timer.at("updateGL");
#endif

    //image preloading
    if (file_idx < ((unsigned int)files.count()-1)) {
        loadImage(file_idx+1);
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
void ViewerWidget::zoom(int mdelta, QPoint pos, float factor)
{
    if (mdelta==0) {
        //do nothing
        return;
    }

    if (mdelta > 0) {
        //multiplicator for zooming in
        delta=factor;
    }

    if (mdelta < 0) {
        //multiplicator for zooming out
        delta=2.0-factor;
    }

    texture->zoom(delta,pos);
    updateGL();
}


/*!
    \fn ViewerWidget::mouseDoubleClickEvent(QMouseEvent * e )
    a double click resets the view (zoom and move)
 */
void ViewerWidget::mouseDoubleClickEvent(QMouseEvent * )
{
    texture->reset();
    updateGL();
}


/*!
    \fn ViewerWidget::mouseReleaseEvent(QMouseEvent * e)
 */
void ViewerWidget::mouseReleaseEvent(QMouseEvent * )
{
    timerMouseMove.start(2000);
    unsetCursor();
    if (texture->setSize(QSize(0,0))) { //load full resolution image
        downloadTex(texture);
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
    \fn ViewerWidget::getOGLstate()
    check if OpenGL engine is ready. This function is called from outside the widget.
    If OpenGL doen't work correctly, the widget can be destroyed
    \return OGLstate::oglNoContext No OpenGl context could be retrieved
    \return OGLstate::oglNoRectangularTexture GLGL_ARB_texture_rectangle is not supported
    \return OGLstate::oglOK all is fine
 */
OGLstate ViewerWidget::getOGLstate()
{
    //no OpenGL context is found. Are the drivers ok?
    if ( !isValid() ) {
        return oglNoContext;
    }

    //GL_ARB_texture_rectangle is not supported
    QString s = QString ( ( char* ) glGetString ( GL_EXTENSIONS ) );
    if ( !s.contains ( "GL_ARB_texture_rectangle",Qt::CaseInsensitive ) )	{
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
bool ViewerWidget::isReallyFullScreen()
{
    return (width()==screen_width);
}
