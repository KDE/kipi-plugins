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

#ifndef TEXTURE_H
#define TEXTURE_H

#define QT_CLEAN_NAMESPACE
#include <qgl.h>
#include <qstring.h>
#include <qdatetime.h>
#include <iostream>
#include <qimage.h>

/**
 * @short Texture class
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.1
 */


class Texture{
public:
	enum Type {QT, JPEGLIB};
	enum Quality {hi,lo, hi_noalpha};
    Texture();
    ~Texture();
    int height();
    int width();
    bool load(QString fn, QSize size, GLuint tn);
	bool load(QImage im, QSize size, GLuint tn);
    GLvoid * data();
    GLuint texnr();
    Quality quality();
    GLfloat vertex_bottom();
	GLfloat vertex_top();
    GLfloat vertex_left();
    GLfloat vertex_right();
    void setViewport(int w, int h);
    void zoom(float delta, QPoint mousepos);
    void reset();
    void move(QPoint diff);
    bool setSize(QSize size);
    void rotate();
	void zoomToOriginal();
	
protected:
	bool _load();
	void calcVertex();
    
	
	int display_x, display_y;
	Type type;
	Quality _quality;
	GLuint _texnr;
	QSize initial_size;
    QString filename;
    QImage qimage, glimage;
    unsigned char * imageJPEGLIB;
    unsigned char * data_noalpha;
	float rdx,rdy,z,ux,uy,rtx,rty;
	float vtop, vbottom, vleft, vright;
	int rotate_list[4], rotate_idx;
};

#endif
