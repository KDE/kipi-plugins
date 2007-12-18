/***************************************************************************
 *   Copyright (C) 2007 by Markus Leuthold   *
 *   <kusi (+at) forum.titlis.org>   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.        *
 ***************************************************************************/

#undef PERFORMANCE_ANALYSIS

// global includes
#include <qgl.h>
#include <iostream>
#include <qlabel.h>
#include <klocale.h>
#include <qdragobject.h>
#include <qbitmap.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

// local includes.
#include "ogl.h"
#include "texture.h"
#include "help.h"
#ifdef PERFORMANCE_ANALYSIS
	#include "timer.h"
#endif
#include "ogl.moc"

// using namespace std;
using namespace KIPIviewer;

ogl::ogl(KIPI::Interface* interface) {
		KIPI::ImageCollection selection = interface->currentSelection();
		KIPI::ImageCollection album = interface->currentAlbum();

		KURL::List myfiles; //pics which are displayed in imageviewer
		QString selectedImage; //selected pic in hostapp

		int foundNumber=0;
		file_idx=0; //index of picture to be displayed

		if ( selection.images().count()==0 ) {
			kdDebug(51000) << "no image selected, load entire album" << endl;
			myfiles = album.images();
		} 
		else if ( selection.images().count()==1 ) {
			kdDebug(51000) << "one image selected, load entire album and start with selected image" << endl;
			selectedImage = selection.images().first().path();
			myfiles = album.images();
		} 
		else if ( selection.images().count()>1 ) {
			kdDebug(51000) << "load " << selection.images().count() << " selected images" << endl;
			myfiles = selection.images();
		}

		// populate QStringList::files
		for(KURL::List::Iterator it = myfiles.begin(); it != myfiles.end();it++) {
			
			// find selected image in album in order to determine the first displayed image
			// in case one image was selected and the entire album was loaded
			QString s = (*it).path();
			if ( s==selectedImage ) {
				
				kdDebug(51000) << "index for " << selectedImage << " = " << foundNumber << endl;
				file_idx=foundNumber;
			}
			
			// only add images to files
			KMimeType::Ptr type = KMimeType::findByURL(s);
			bool isImage=type->name().find("image")>=0;
			
			if ( isImage ) {
				files.append(s);
				foundNumber++;  //counter for searching the start image in case one image is selected
				kdDebug(51000) << s << " type=" << type->name() << endl;
			}
		}
		
		firstImage=true;
		kdDebug(51000) << files.count() << "images loaded" << endl;
		
		// initialize cache
		for(int i=0;i<CACHESIZE;i++) {
			cache[i].file_index=EMPTY;
			cache[i].texture=new Texture();
		}

		// define zoomfactors for one zoom step
		zoomfactor_scrollwheel = 1.1;
		zoomfactor_mousemove = 1.03;
		zoomfactor_keyboard = 1.05;
		
		// load cursors for zooming and panning
		QString file;
		file = locate( "data", "kipiplugin_imageviewer/pics/zoom.png" );
		zoomCursor=QCursor(file);
		file = locate( "data", "kipiplugin_imageviewer/pics/hand.png" );
		moveCursor=QCursor(file);
		
		// get path of nullImage in case QImage can't load the image
		nullImage = locate( "data", "kipiplugin_imageviewer/pics/nullImage.png" );
		
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
	\fn ogl::initializeGL()
	\todo blending
 */
void ogl::initializeGL() {	
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
}


/*!
	\fn ogl::paintGL()
 */
void ogl::paintGL() {	
	//prepare 1st image
	//this test has to be performed here since QWidget::width() is only updated now
	if (firstImage) {
		texture=loadImage(file_idx);
		texture->reset();
		downloadTex(texture);
		firstImage=false;
	}	
		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-5.0f);
	drawImage(texture);
}




/*!
	\fn ogl::resizeGL(int w, int h)
 */
void ogl::resizeGL(int w, int h)
{
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
	
	if (!firstImage) {
		texture->setViewport(w,h);
	}
}





/*!
	\fn ogl::drawImage(Texture * texture)
	\brief render the image
 */
void ogl::drawImage(Texture * texture)
{
// 	cout << "enter drawImage: target=" << texture->texnr() << " dim=" << texture->height() << " " << texture->width() << endl;	
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
	\fn ogl::keyPressEvent(QKeyEvent *k)
	Handle all keyboard events. All events which are not handled trigger 
	a help window.
 */
void ogl::keyPressEvent(QKeyEvent *k)
{
	QPoint middlepoint;

    switch (k->key()) {
		// next image
        case Key_N:
		case Key_Right:
		case Key_Down:
		case Key_PageDown:
		case Key_Space:
			nextImage();
			break;
		
		// previous image
        case Key_P:
		case Key_Left:
		case Key_Up:
		case Key_PageUp:
			prevImage();
			break;	
				
		// rotate image
		case Key_R:
			texture->rotate();
			downloadTex(texture);
			updateGL();
			break;		
			
		// terminate image viewer
		case Key_Escape:
			// clean up: where does this have to be done?
			close(true);
			break;
			
		// full screen
		case Key_F:
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
		case Key_Z:
			texture->reset();
			updateGL();
			break;
			
		// toggle permanent between "show next image" and "zoom" on mousewheel change		
		case Key_C:
			if (wheelAction==zoomImage)
				wheelAction=changeImage;
			else
				wheelAction=zoomImage;
			break;
		
		// zoom	in
		case Key_Plus:
			middlepoint =  QPoint(width()/2,height()/2);
			if (texture->setSize( zoomsize )) {
				downloadTex(texture); //load full resolution image
			};
			zoom(-1, middlepoint, zoomfactor_keyboard);	
			break;
			
		// zoom out
		case Key_Minus:
			middlepoint =  QPoint(width()/2,height()/2);
			if (texture->setSize( zoomsize )) 
				downloadTex(texture); //load full resolution image
			zoom(1, middlepoint, zoomfactor_keyboard);	
			break;
			
		// zoom to original size
		case Key_O:
			texture->zoomToOriginal();
			updateGL();
			break;
		
		// toggle temorarily between "show next image" and "zoom" on mousewheel change
		case Key_Control:
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
		case Key_Shift:
			break;

		//key is not bound to any action, therefore show help dialog to enlighten the user
		default:
			QDialog * h = new HelpDialog(0,0,Qt::WStyle_StaysOnTop);
			h->show();
	}
}

/*!
    \fn ogl::keyReleaseEvent ( QKeyEvent * e )
 */
void ogl::keyReleaseEvent ( QKeyEvent * e )
{
	switch (e->key()) {
		case Key_Plus:
		case Key_Minus:
			if (!e->isAutoRepeat()) {
				unsetCursor();
				if (texture->setSize(QSize(0,0))) {
					downloadTex(texture); //load full resolution image
				}
				updateGL();
			} else
				e->ignore();
			break;
			
		case Key_Control:
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
	\fn ogl::downloadTex(Texture * tex)
	download texture to video memory
 */
void ogl::downloadTex(Texture * tex)
{
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, tex->texnr());
	// glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);
	// glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);
	glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, tex->width(), tex->height(), 0,GL_RGBA, GL_UNSIGNED_BYTE, tex->data());			
}



/*!
	\fn ogl::loadImage(int file_index)
	\param file_index index to QStringList files
    load files[file_index] into a texture object if it is not already cached
 */
Texture * ogl::loadImage(int file_index)
{
	int imod=file_index%CACHESIZE; //index for cache

	if (cache[imod].file_index==file_index){ 
		//image is already cached	
		kdDebug(51000) << "image " << file_index << " is already in cache@" << imod << endl;
		return cache[imod].texture;

	} else {
		// image is net yet loaded
		QString f = files[file_index];
		kdDebug(51000) << "loading image " << f << "(idx=" << file_index << ") to cache@" << imod << endl;
		cache[imod].file_index=file_index;

		// handle non-loadable images
		if (!cache[imod].texture->load(f,QSize(width(),height()),tex[0])) {
			cache[imod].texture->load(nullImage,QSize(width(),height()),tex[0]);
		}

		cache[imod].texture->setViewport(width(),height());
		return cache[imod].texture;
	}
}


/*!
	\fn ogl::wheelEvent ( QWheelEvent * e )
 */
void ogl::wheelEvent ( QWheelEvent * e )
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
	\fn ogl::mousePressEvent ( QMouseEvent * e )
 */
void ogl::mousePressEvent ( QMouseEvent * e )
{	
	// begin zoom
	// scale down texture  for fast zooming
	// texture	 will be set to original size on mouse up
	if (texture->setSize( zoomsize )) { //load downsampled image
		downloadTex(texture);
	}
	
	timerMouseMove.stop(); //user is something up to, therefore keep the cursor
	if ( e->button() == LeftButton ) {
		setCursor (moveCursor); //defined in constructor
	}
	
	if ( e->button() == RightButton ) {
		setCursor (zoomCursor);
	}
	
	startdrag=e->pos();
	previous_pos=e->pos();
}


/*!
	\fn ogl::mouseMoveEvent ( QMouseEvent * e ) 
 */
void ogl::mouseMoveEvent ( QMouseEvent * e ) 
{
	if ( e->state() == LeftButton ) {
		//panning
		QPoint diff=e->pos()-startdrag;
		texture->move(diff);
		updateGL();
		startdrag=e->pos();
	} else if ( e->state() == RightButton ) {
		//zooming
		zoom(previous_pos.y()-e->y(), startdrag, zoomfactor_mousemove );
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
	\fn ogl::prevImage()
 */
void ogl::prevImage()
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
	\fn ogl::nextImage()
 */
void ogl::nextImage()
{
#ifdef PERFORMANCE_ANALYSIS
	Timer timer;
#endif
	if (file_idx<(files.count()-1))
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
	if (file_idx<(files.count()-1)) {
		loadImage(file_idx+1);
#ifdef PERFORMANCE_ANALYSIS
		timer.at("preloading");
#endif
	}
}


/*!
	\fn ogl::zoom(int mdelta, QPos pos)
	\param mdelta delta of mouse movement: 
	                            mdelta>0: zoom in
	                            mdelta<0: zoom out
	                            mdelta=0: do nothing
	\param pos position of mouse
	\param factor zoom factor:scrollwheel needs a higher factor that right click mouse move. factor=1 -> no zoom
 */
void ogl::zoom(int mdelta, QPoint pos, float factor)
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
	\fn ogl::mouseDoubleClickEvent(QMouseEvent * e )
	a double click resets the view (zoom and move)
 */
void ogl::mouseDoubleClickEvent(QMouseEvent * )
{
	texture->reset();
	updateGL();
}


/*!
	\fn ogl::mouseReleaseEvent(QMouseEvent * e)
 */
void ogl::mouseReleaseEvent(QMouseEvent * )
{
	timerMouseMove.start(2000);
	unsetCursor();
	if (texture->setSize(QSize(0,0))) { //load full resolution image
		downloadTex(texture);
	}
	updateGL();
}


/*!
	\fn ogl::timeoutMouseMove()
	being called if user didn't move the mouse for longer than 2 sec
 */
void ogl::timeoutMouseMove()
{
	setCursor (QCursor (blankCursor));
}



/*!
	\fn ogl::getOGLstate()
	check if OpenGL engine is ready. This function is called from outside the widget.
	If OpenGL doen't work correctly, the widget can be destroyed
	\return OGLstate::oglNoContext No OpenGl context could be retrieved
	\return OGLstate::oglNoRectangularTexture GLGL_ARB_texture_rectangle is not supported
	\return OGLstate::oglOK all is fine
 */
OGLstate ogl::getOGLstate()
{
	//no OpenGL context is found. Are the drivers ok?
	if ( !isValid() ) {
		return oglNoContext;
	}
	
	//GL_ARB_texture_rectangle is not supported
	QString s = QString ( ( char* ) glGetString ( GL_EXTENSIONS ) );
	if ( !s.contains ( "GL_ARB_texture_rectangle",false ) )	{
		return oglNoRectangularTexture;
	}
	
	//everything is ok!
	return oglOK;
}
