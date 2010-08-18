/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a widget to preview image effect.
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PREVIEW_IMAGE_H
#define PREVIEW_IMAGE_H

// Qt includes

#include <QGraphicsView>
#include <QString>
#include <QColor>

// Local includes

#include "kipiplugins_export.h"

class QResizeEvent;
class QWheelEvent;
class QMouseEvent;
class QEvent;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT PreviewImage : public QGraphicsView
{
    Q_OBJECT

public:

    PreviewImage(QWidget* parent);
    ~PreviewImage();

    bool load(const QString& file);
    bool setImage(const QImage& img);

public Q_SLOTS:

    void slotZoomIn();
    void slotZoomOut();
    void slotZoom2Fit();

protected:

    void wheelEvent(QWheelEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    bool eventFilter(QObject*, QEvent*);

private:

    class PreviewImagePriv;
    PreviewImagePriv* const d;
};

} // namespace KIPIPlugins

#endif /* PREVIEW_IMAGE_H */
