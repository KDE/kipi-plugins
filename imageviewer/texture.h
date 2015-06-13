/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef TEXTURE_H
#define TEXTURE_H

// Qt includes

#include <QGLWidget>
#include <QImage>
#include <QString>

// Local includes

#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIViewerPlugin
{

class Texture
{

public:

    Texture();
    ~Texture();

    int  height() const;
    int  width()  const;

    bool load(const QString& fn, const QSize& size, GLuint tn);
    bool load(const QImage& im, const QSize& size, GLuint tn);

    GLvoid* data()          const;
    GLuint  texnr()         const;
    GLfloat vertex_bottom() const;
    GLfloat vertex_top()    const;
    GLfloat vertex_left()   const;
    GLfloat vertex_right()  const;

    void setViewport(int w, int h);
    void zoom(float delta, const QPoint& mousepos);
    void reset();
    void move(const QPoint& diff);
    bool setSize(QSize size);
    void rotate();
    void zoomToOriginal();

private:

    bool loadInternal();
    void calcVertex();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIViewerPlugin

#endif // TEXTURE_H
