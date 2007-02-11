/***************************************************************************
 *   Copyright (C) 2007 by Markus Leuthold   *
 *   kusi (+at) forum.titlis.org   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// #define QT_CLEAN_NAMESPACE
#include <qgl.h>
#include <iostream>
#include <qlabel.h>
#include <klocale.h>
#include <qdragobject.h>
#include <qbitmap.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>				 


#include "ogl.h"
#include "timer.h"
#include "texture.h"
				 
using namespace std;

		 
ogl::ogl(KIPI::Interface* interface) {
		KIPI::ImageCollection selection = interface->currentSelection();
		KIPI::ImageCollection album = interface->currentAlbum();
		KURL::List myfiles;

		QString selectedImage;
		int foundNumber=0;
		file_idx=0; //index of picture to be displayed

		if (selection.images().count()==0) {
			kdDebug(51000) << "no image selected, load entire album" << endl;
			myfiles = album.images();
		} 
		else if (selection.images().count()==1) {
			kdDebug(51000) << "one image selected, load entire album and start with selected image" << endl;
			selectedImage = selection.images().first().path();
			myfiles = album.images();
		} 
		else if (selection.images().count()>1) {
			kdDebug(51000) << "load " << selection.images().count() << " selected images" << endl;
			myfiles = selection.images();
		}

		for(KURL::List::Iterator it = myfiles.begin(); it != myfiles.end();it++) {
				QString s = (*it).path(); 
				if (s==selectedImage) {
					//find selected image in album in order to determine the starting image
// 					kdDebug(51000) << "index for " << selectedImage << " = " << foundNumber << endl;
					file_idx=foundNumber;
				}
				
				//TODO also handle movies
				KMimeType::Ptr type = KMimeType::findByURL(s);
				if (type->name().find("image")>=0) {
					files.append(s);
					foundNumber++;  //counter for searching the start image in case one  image is selected
					kdDebug(51000) << s << " type=" << type->name() << endl;
			}
			
		}
		
		firstImage=true;
		kdDebug(51000) << files.count() << "images loaded" << endl;
		
		//initialize cache
		for(int i=0;i<CACHESIZE;i++) {
			cache[i].file_index=EMPTY;
			cache[i].texture=new Texture();
		}
		wheelAction = changeImage;
		zoomfactor_scrollwheel = 1.1;
		zoomfactor_mousemove = 1.03;
		zoomfactor_keyboard = 1.05;
		
		//load cursors for zooming and panning
		QString file;
		file = locate( "data", "kipiplugin_imageviewer/pics/zoom.png" );
		zoomCursor=QCursor(file);
		file = locate( "data", "kipiplugin_imageviewer/pics/hand.png" );
		moveCursor=QCursor(file);

		showFullScreen(); 
		
		//let the cursor dissapear after 2sec of inactivity
		connect( &timerMouseMove, SIGNAL(timeout()),this, SLOT( timeoutMouseMove()) );
		timerMouseMove.start(2000);
		setMouseTracking(true);
}		 
		 

/*!
    \fn ogl::initializeGL()
	\todo blending
 */
void ogl::initializeGL() {
//     cout << "enter initializeGL" << endl;

    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    // Clear The Background Color
    glClearColor(0.0, 0.0, 0.0, 1.0f);
    // Turn Blending On
    glEnable(GL_BLEND);
    // Blending Function For Translucency Based On Source Alpha Value
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // Enable perspective vision
    glClearDepth(1.0f);

    oldidx=99999;

	glGenTextures(1, tex);
	zoomsize=QSize(1024,768);
// 		zoomsize=QSize(400,200); 
}


/*!
    \fn ogl::paintGL()
 */
void ogl::paintGL() {
// 	cout << "enter paintGL" << endl;
	if (firstImage) {		//this test has to be performed here since QWidget::width() is only updated now
		//prepare 1st image
// 		cout << "file_idx=" << file_idx << endl;
		texture=loadImage(file_idx);
		texture->reset();
		downloadTex(texture);
		firstImage=false;
	}	
		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-5.0f);
	drawImage(texture);
// 	cout << "exit paintGL" << endl;
}




/*!
    \fn ogl::resizeGL(int w, int h)
 */
void ogl::resizeGL(int w, int h)
{
// 	cout <<  "enter resizeGL"<< endl;

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
// 	cout << "ResizeGL:" << width() << endl;
// 	cout << "exit resizeGL" << endl;
}


#include "ogl.moc"


/*!
    \fn ogl::drawImage(int i)
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
 */
void ogl::keyPressEvent(QKeyEvent *k)
{
	Timer timer;
	QPoint middlepoint;
    switch (k->key()) {
        case Key_N:
		case Key_Right:
		case Key_Down:
		case Key_PageDown:
		case Key_Space:
			nextImage();
			break;
        case Key_P:
		case Key_Left:
		case Key_Up:
		case Key_PageUp:
			prevImage();
			break;		
		case Key_R:
			texture->rotate();
			downloadTex(texture);
			updateGL();
			break;		
		case Key_Escape:
			// clean up: where does this have to be done?
			close(true);
			break;
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
		case Key_Z:
			texture->reset();
			updateGL();
			break;
		case Key_C:
			if (wheelAction==zoomImage)
				wheelAction=changeImage;
			else
				wheelAction=zoomImage;
			break;
		case Key_Plus:
			middlepoint =  QPoint(width()/2,height()/2);
			if (texture->setSize( zoomsize )) {
				downloadTex(texture); //load full resolution image
			};
			zoom(-1, middlepoint, zoomfactor_keyboard);	
			break;
		case Key_Minus:
			middlepoint =  QPoint(width()/2,height()/2);
			if (texture->setSize( zoomsize )) 
				downloadTex(texture); //load full resolution image
			zoom(1, middlepoint, zoomfactor_keyboard);	
			break;
		case Key_O:
			texture->zoomToOriginal();
			updateGL();
	}
}




/*!
    \fn ogl::downloadTex(Texture * tex)
    download texture to video memory
    \todo get rid of quality

 */
void ogl::downloadTex(Texture * tex)
{
	GLenum format;
// 	cout << "enter downloadTex: " << endl;
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, tex->texnr());
	switch (tex->quality()) {
		case Texture::hi_noalpha:
			format=GL_RGBA;
			break;
		default:
			format=GL_RGB;
			break;
	}
// 	glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);
//     glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);
	glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, format, tex->width(), tex->height(), 0,GL_RGBA, GL_UNSIGNED_BYTE, tex->data());		
			
}


/*!
    \fn ogl::transition()
    \todo everything
 */
void ogl::transition(int i, int j)
{
/*	
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, t1);
	glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, imMini[i].width(), imMini[i].height(), 0,GL_RGBA, GL_UNSIGNED_BYTE, imMini[i].bits() );
	
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, t2);
	glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, imMini[j].width(), imMini[j].height(), 0,GL_RGBA, GL_UNSIGNED_BYTE, imMini[j].bits() );
	
	
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-6.0f);
// 	glEnable(GL_BLEND); 
	glEnable(GL_DEPTH_TEST);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
// 		cout << z << endl;
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, t1);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(-1, -1, 0);
		
			glTexCoord2f(imMini[i].width(), 0);
			glVertex3f(1, -1, 0);
		
			glTexCoord2f(imMini[i].width(), imMini[i].height());
			glVertex3f(1, 1, 0);
		
			glTexCoord2f(0, imMini[i].height());
			glVertex3f(-1.0, 1, 0);
		glEnd(); 
	
		glLoadIdentity();
		
		glTranslatef(0.0f,0.0f,-6.0);
		glTranslatef(0.0f,0.0f,zzz);
		glRotatef(10,0,1,0);
		
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, t2);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(-1, -1, 0);
		
			glTexCoord2f(imMini[j].width(), 0);
			glVertex3f(1, -1, 0);
		
			glTexCoord2f(imMini[j].width(), imMini[j].height());
			glVertex3f(1, 1, 0);
		
			glTexCoord2f(0, imMini[j].height());
			glVertex3f(-1.0, 1, 0);
		glEnd(); 
		glFinish();

//    glDisable(GL_BLEND); 
   glDisable(GL_DEPTH_TEST);
	*/
	
}


/*!
    \fn ogl::loadImage(int file_index)
    load files[file_index] into a texture object if it is not already cached
 */
Texture * ogl::loadImage(int file_index)
{
// 	cout << "enter loadImage" << " w=" << width() << " h=" << height() << endl;	
	int imod=file_index%CACHESIZE; //index for cache
// 	cout << "cache index=" << imod << " file_index=" << file_index << endl;
	if (cache[imod].file_index==file_index){ 
		kdDebug(51000) << "image " << file_index << " already loaded" << endl;
		return cache[imod].texture;
	} else {
		QString f = files[file_index];
		kdDebug(51000) << "loading " << f << " file_index=" << file_index << endl;
		cache[imod].file_index=file_index;
		cache[imod].texture->load(f,QSize(width(),height()),tex[0]);
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
		case zoomImage:
			setCursor(zoomCursor);
			zoom(e->delta(), e->pos(), zoomfactor_scrollwheel);
			break;
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
		QPoint diff=e->pos()-startdrag;
		texture->move(diff);
		updateGL();
		startdrag=e->pos();
	} else if ( e->state() == RightButton ) {
		zoom(previous_pos.y()-e->y(), startdrag, zoomfactor_mousemove );
		previous_pos=e->pos();
	} else {	
		timerMouseMove.start(2000);
		unsetCursor();
	}
	return;
}




/*!
    \fn ogl::prevImage()
 */
void ogl::prevImage()
{
	Timer timer;
	if (file_idx>0)
		file_idx--;
	else
		return;
	timer.start();
	texture = loadImage(file_idx);
	texture->reset();
	timer.at("loadImage");
	downloadTex(texture);
	timer.at("downloadTex");
	updateGL();
	timer.at("updateGL");
	
	//image preloading
	if (file_idx>0)
		loadImage(file_idx-1);
}


/*!
    \fn ogl::nextImage()
 */
void ogl::nextImage()
{
	Timer timer;
	if (file_idx<(files.count()-1))
		file_idx++;
	else
		return;
	timer.start();
	texture = loadImage(file_idx);
	texture->reset();
	timer.at("loadImage");
	downloadTex(texture);
	timer.at("downloadTex");
	updateGL();
	timer.at("updateGL");
	
	//image preloading
	if (file_idx<(files.count()-1)) {
		loadImage(file_idx+1);
		timer.at("preloading");
	}
}


/*!
    \fn ogl::zoom(int mdelta, QPos pos)
    \param mdelta delta of mouse movement: zoom in if mdelta >0, zoomout otherwise
	\param pos position of mouse
	\param factor zoom factor:scrollwheel needs a higher factor that right click mouse move. factor=1 -> no zoom
 */
void ogl::zoom(int mdelta, QPoint pos, float factor)
{
	if (mdelta > 0)
    	delta=factor; //multiplicator for zooming in
	if (mdelta < 0)
    	delta=2.0-factor; //multiplicator for zooming out
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
    \fn ogl::Cache::keyReleaseEvent ( QKeyEvent * e )
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
		default:
			e->ignore();
			break;
	}
	
}
