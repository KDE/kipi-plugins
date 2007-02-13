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
 *   51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.        *
 ***************************************************************************/

#ifndef _OGL_H_
#define _OGL_H_

// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #endif

#include <qgl.h>
#include <qdir.h>
#include <qimage.h>
#include <qdatetime.h>
#include <iostream>
#include <kurl.h>
#include <kmimetype.h>
#include <qregexp.h>
#include <qcursor.h>
#include <qtimer.h>
#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

#include "texture.h"

/**
 * @short OpenGL widget
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.1
 */
 
#define EMPTY 99999
#define CACHESIZE 3
using namespace std;

class ogl : public QGLWidget
{
    Q_OBJECT
			
public:
	ogl(KIPI::Interface* interface);

	
	~ogl() {
// 		std::cout << "enter ogl destructor" << std::endl;
		glDeleteTextures(1,tex);
		for(int i=0;i<CACHESIZE;i++) {
			cache[i].file_index=EMPTY;
			delete cache[i].texture;
		}
	}
	
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    void drawImage(Texture * tex);
    void downloadTex(Texture * tex);
    void transition(int i, int j);
    Texture * loadImage(int file_index);
    void prevImage();
    void nextImage();
    void zoom(int mdelta, QPoint pos, float factor);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void keyReleaseEvent ( QKeyEvent * e );

protected:
	struct Cache {
		int file_index;
		Texture * texture;
		
	};
	
	enum WheelAction {
		zoomImage, changeImage
	};
	Texture * texture;
	unsigned int old_file_idx,file_idx,idx, oldidx;
	float ratio_view_y,ratio_view_x,delta;
    QTime timer;
    QDir directory;
    QStringList files;
    unsigned char *  imageJPEGLIB;
	Cache cache[CACHESIZE];
	GLuint tex[3];
    float vertex_height,vertex_width,vertex_left,vertex_top,vertex_right,vertex_bottom;
    QPoint startdrag, previous_pos;
	WheelAction wheelAction;
    bool firstImage;
    QSize zoomsize;
    QTimer timerMouseMove;
    QCursor moveCursor, zoomCursor;
	float zoomfactor_scrollwheel,  zoomfactor_mousemove,  zoomfactor_keyboard;

protected:
    virtual void keyPressEvent(QKeyEvent *k);
    virtual void wheelEvent ( QWheelEvent * e );
    virtual void mouseMoveEvent ( QMouseEvent * e );
    virtual void mousePressEvent ( QMouseEvent * e );
    virtual void mouseDoubleClickEvent(QMouseEvent * e );
private slots:
    void timeoutMouseMove();
};

#endif // _OGL_H_
