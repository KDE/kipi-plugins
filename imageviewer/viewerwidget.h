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

#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

// C++ includes

#include <iostream>

// Qt includes

#include <QCursor>
#include <QDir>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

// KDE includes

#include <kurl.h>
#include <kmimetype.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>

// Local includes

#include "texture.h"

/**
 * @short OpenGL widget for image viewer
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.2
 */


//keep in mind that one cache entry takes 20MB for a 5mpix pic
#define CACHESIZE 4
#define EMPTY 99999

namespace KIPIviewer {
using namespace std;

enum OGLstate {
        oglOK, oglNoRectangularTexture, oglNoContext
};

class ViewerWidget : public QGLWidget
{
    Q_OBJECT

public:
	ViewerWidget(KIPI::Interface*);
	~ViewerWidget() {
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
    Texture * loadImage(int file_index);
    void prevImage();
    void nextImage();
    bool listOfFilesIsEmpty() const;
    void zoom(int mdelta, QPoint pos, float factor);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void keyReleaseEvent ( QKeyEvent * e );
    OGLstate getOGLstate();

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
	QString nullImage;
	int screen_width;
	KIPI::Interface * kipiInterface;

protected:
    virtual void keyPressEvent(QKeyEvent *k);
    virtual void wheelEvent ( QWheelEvent * e );
    virtual void mouseMoveEvent ( QMouseEvent * e );
    virtual void mousePressEvent ( QMouseEvent * e );
    virtual void mouseDoubleClickEvent(QMouseEvent * e );
    bool isReallyFullScreen();

private Q_SLOTS:
    void timeoutMouseMove();
};

} //namespace KIPIviewer

#endif // VIEWERWIDGET_H
