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

#include "previewimage.moc"

// Qt includes

#include <QAction>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QScrollBar>
#include <QToolBar>

// KDE includes

#include <klocale.h>
#include <kiconloader.h>
#include <kicon.h>
#include <kvbox.h>

namespace KIPIPlugins
{

class PreviewImage::PreviewImagePriv
{

public:

    PreviewImagePriv()
    {
        pixmapItem     = 0;
        scene          = 0;
        zoomInAction   = 0;
        zoomOutAction  = 0;
        zoom2FitAction = 0;
        toolBar        = 0;
    }

    int                  lastdx;
    int                  lastdy;

    QGraphicsScene*      scene;
    QGraphicsPixmapItem* pixmapItem;

    QAction*             zoomInAction;
    QAction*             zoomOutAction;
    QAction*             zoom2FitAction;

    QToolBar*            toolBar;
};

PreviewImage::PreviewImage(QWidget* parent)
            : QGraphicsView(parent), d(new PreviewImagePriv)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setCacheMode(QGraphicsView::CacheBackground);

    d->scene      = new QGraphicsScene;
    d->pixmapItem = new QGraphicsPixmapItem;

    d->scene->addItem(d->pixmapItem);
    setScene(d->scene);

    // create context menu

    d->zoomInAction = new QAction(KIcon("zoom-in"), i18n("Zoom In"), this);
    d->zoomInAction->setToolTip(i18n("Zoom In"));
    d->zoomInAction->setShortcut(Qt::Key_Plus);
    connect(d->zoomInAction, SIGNAL(triggered()),
            this, SLOT(slotZoomIn()));

    d->zoomOutAction = new QAction(KIcon("zoom-out"), i18n("Zoom Out"), this);
    d->zoomOutAction->setToolTip(i18n("Zoom Out"));
    d->zoomOutAction->setShortcut(Qt::Key_Minus);
    connect(d->zoomOutAction, SIGNAL(triggered()),
            this, SLOT(slotZoomOut()));

    d->zoom2FitAction = new QAction(KIcon("zoom-fit-best"), i18n("Zoom to Fit"), this);
    d->zoom2FitAction->setToolTip(i18n("Zoom to Fit"));
    d->zoom2FitAction->setShortcut(Qt::Key_Asterisk);
    connect(d->zoom2FitAction, SIGNAL(triggered()),
            this, SLOT(slotZoom2Fit()));

    addAction(d->zoomInAction);
    addAction(d->zoomOutAction);
    addAction(d->zoom2FitAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    // Create ToolBar

    d->toolBar = new QToolBar(this);
    d->toolBar->addAction(d->zoomInAction);
    d->toolBar->addAction(d->zoomOutAction);
    d->toolBar->addAction(d->zoom2FitAction);
    d->toolBar->hide();
    d->toolBar->installEventFilter(this);

    horizontalScrollBar()->installEventFilter(this);
    verticalScrollBar()->installEventFilter(this);
}

PreviewImage::~PreviewImage()
{
    delete d;
}

bool PreviewImage::setImage(const QImage& img)
{
    if (!img.isNull())
    {
        d->pixmapItem->setPixmap(QPixmap::fromImage(img));
        d->pixmapItem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
        d->scene->setSceneRect(0, 0, img.width(), img.height());
        return true;
    }

    return false;
}

bool PreviewImage::load(const QString& file)
{
    QImage image(file);
    return setImage(image);
}

void PreviewImage::slotZoomIn()
{
    scale(1.5, 1.5);
    d->zoom2FitAction->setDisabled(false);
}

void PreviewImage::slotZoomOut()
{
    scale(1.0 / 1.5, 1.0 / 1.5);
    d->zoom2FitAction->setDisabled(false);
}

void PreviewImage::slotZoom2Fit()
{
    fitInView(d->pixmapItem->boundingRect(), Qt::KeepAspectRatio);
    d->zoom2FitAction->setDisabled(true);
}

void PreviewImage::wheelEvent(QWheelEvent *e)
{
    if(e->modifiers() == Qt::ControlModifier)
    {
        if(e->delta() > 0)
        {
            slotZoomIn();
        }
        else
        {
            slotZoomOut();
        }
    }
    else
    {
        QGraphicsView::wheelEvent(e);
    }
}

void PreviewImage::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        d->lastdx = e->x();
        d->lastdy = e->y();
        setCursor(Qt::ClosedHandCursor);
    }
}

void PreviewImage::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        unsetCursor();
}

void PreviewImage::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        int dx    = e->x() - d->lastdx;
        int dy    = e->y() - d->lastdy;
        verticalScrollBar()->setValue(verticalScrollBar()->value() - dy);
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - dx);
        d->lastdx = e->x();
        d->lastdy = e->y();
    }
    else
    {
        if (verticalScrollBar()->isVisible() || horizontalScrollBar()->isVisible())
            setCursor(Qt::OpenHandCursor);
    }
}

void PreviewImage::enterEvent(QEvent*)
{
    d->toolBar->show();
}

void PreviewImage::leaveEvent(QEvent*)
{
    d->toolBar->hide();
}

bool PreviewImage::eventFilter(QObject *obj, QEvent *ev)
{
    if ( obj == d->toolBar )
    {
        if ( ev->type() == QEvent::Enter)
            setCursor(Qt::ArrowCursor);
        else if ( ev->type() == QEvent::Leave)
            unsetCursor();

        return false;
    }
    else if ( obj == verticalScrollBar() && verticalScrollBar()->isVisible())
    {
        if ( ev->type() == QEvent::Enter)
            setCursor(Qt::ArrowCursor);
        else if ( ev->type() == QEvent::Leave)
            unsetCursor();

        return false;
    }
    else if ( obj == horizontalScrollBar() && horizontalScrollBar()->isVisible())
    {
        if ( ev->type() == QEvent::Enter)
            setCursor(Qt::ArrowCursor);
        else if ( ev->type() == QEvent::Leave)
            unsetCursor();

        return false;
    }

    return QGraphicsView::eventFilter(obj, ev);
}

} // namespace KIPIPlugins
