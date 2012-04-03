/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef TEXTURE_H
#define TEXTURE_H

// Qt includes

#include <QGLWidget>
#include <QImage>
#include <QString>

// LibKipi includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes

#include "kpmetadata.h"

using namespace KIPI;
using namespace KIPIPlugins;

/**
 * @short Texture class
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.2
 */

namespace KIPIViewerPlugin
{

class Texture
{

public:

    Texture(Interface* const iface);
    ~Texture();

    int  height() const;
    int  width()  const;

    bool load(const QString& fn, const QSize& size, GLuint tn);
    bool load(const QImage& im, const QSize& size, GLuint tn);

    GLvoid* data();
    GLuint  texnr() const;
    GLfloat vertex_bottom() const;
    GLfloat vertex_top() const;
    GLfloat vertex_left() const;
    GLfloat vertex_right() const;

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

    int                          display_x, display_y;
    GLuint                       _texnr;
    QString                      filename;
    QImage                       qimage, glimage;
    float                        rdx, rdy, z, ux, uy, rtx, rty;
    float                        vtop, vbottom, vleft, vright;
    QSize                        m_initial_size;
    KPMetadata::ImageOrientation m_rotate_list[4];
    int                          m_rotate_idx;
    Interface*                   m_kipiInterface;
};

} // namespace KIPIViewerPlugin

#endif // TEXTURE_H
